#include "MonitorCore.h"
#include "RegistryManager.h"
#include "Utils.h"
#include "HardwareID.h"
#include <iostream>
#include <windows.h>
#include <tlhelp32.h> // for process enumeration

// 模拟的密钥验证（实际应该更复杂）
#define PERMANENT_KEY L"FINAL-SETTLEMENT-KEY-001"
#define ONETIME_KEY_PREFIX L"TEMP-ACCESS-"

// --------------------------- 初始化 ---------------------------

bool MonitorCore::InitRegistryData(const std::wstring& targetName, const std::wstring& expiryDate) {
	std::wstring currentDate = Utils::GetCurrentDateString();

	// 写入核心数据
	bool ok = RegistryManager::WriteString(L"TargetName", targetName);
	ok &= RegistryManager::WriteString(L"ExpiryDate", expiryDate);
	// 写入初始的 LastKnownDate，作为防篡改的基准
	ok &= RegistryManager::WriteString(L"LastKnownDate", currentDate);
	// 写入初始状态：试用中
	ok &= RegistryManager::WriteDword(L"KeyStatus", KeyStatus::Trial);

	return ok;
}

// --------------------------- 日期和状态检查 ---------------------------

bool MonitorCore::CheckDatesAndState(std::wstring& targetName) {
	long currentDateLong = Utils::StringDateToLong(Utils::GetCurrentDateString());

	std::wstring expiryDateStr, lastDateStr;
	DWORD status = KeyStatus::Trial;

	// 1. 读取注册表数据
	RegistryManager::ReadString(L"TargetName", targetName);
	RegistryManager::ReadString(L"ExpiryDate", expiryDateStr);
	RegistryManager::ReadString(L"LastKnownDate", lastDateStr);
	RegistryManager::ReadDword(L"KeyStatus", status);

	// 如果是永久激活，则停止监控并返回
	if (status == KeyStatus::PermanentActive) {
		return false;
	}

	long expiryDateLong = Utils::StringDateToLong(expiryDateStr);
	long lastDateLong = Utils::StringDateToLong(lastDateStr);

	// 2. 关键检查 1: 日期回溯 (防篡改)
	if (currentDateLong < lastDateLong) {
		std::wcout << L"[CORE] 🚨 检测到系统时间被修改 (日期回溯)！" << std::endl;
		return true; // 应该被冻结
	}

	// 3. 关键检查 2: 试用期到期
	if (currentDateLong > expiryDateLong) {
		std::wcout << L"[CORE] 🚫 试用期已到期！" << std::endl;
		return true; // 应该被冻结
	}

	// 4. 正常情况：更新 LastKnownDate (定时更新)
	if (currentDateLong > lastDateLong) {
		RegistryManager::WriteString(L"LastKnownDate", Utils::GetCurrentDateString());
	}

	std::wcout << L"[CORE] ✅ 状态正常，持续监控中..." << std::endl;
	return false; // 不应该被冻结
}

// --------------------------- 窗口冻结 ---------------------------

// 尝试获取目标进程的第一个窗口句柄
HWND GetTargetWindow(DWORD processID) {
	// 这是一个简化版本，实际应用可能需要更复杂的遍历
	// 例如使用 EnumWindows 配合 GetWindowThreadProcessId
	return FindWindow(NULL, NULL); // 无法可靠地通过 PID 获取主窗口，此处仅作演示
}


void MonitorCore::FreezeWindowAndPrompt(const std::wstring& targetName) {
	// 警告：通过进程名称找到窗口句柄是复杂的任务，这里使用简化方法。
	// 实际应使用 EnumWindows 遍历所有窗口，通过 GetWindowThreadProcessId 找到对应的进程ID，再与目标进程ID匹配。

	// 1. 尝试找到目标进程的窗口句柄 (简化演示)
	// 假设 targetName 是窗口标题的一部分，或使用特定的类名。
	HWND hWnd = FindWindow(NULL, targetName.c_str());
	// 由于目标是.exe名称，FindWindow可能失败。但为了演示流程，我们继续。

	if (hWnd != NULL) {
		// 2. 冻结窗口
		EnableWindow(hWnd, FALSE);
		std::wcout << L"[FREEZE] 窗口已冻结。" << std::endl;

		// 3. 给出提示框
		MessageBox(hWnd,
			L"本软件试用期已到或检测到系统时间异常。\n请联系客服获取密钥。",
			L"授权警告",
			MB_ICONSTOP | MB_OK);
	}
	else {
		std::wcout << L"[FREEZE] 警告：未能找到目标软件窗口句柄。" << std::endl;
	}
}

// --------------------------- 进程监控 ---------------------------

void MonitorCore::ProcessMonitor(const std::wstring& targetName, bool shouldBeFrozen) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pe)) {
		do {
			// 将 wchar_t* 转换为 std::wstring 并进行比较
			std::wstring currentProcName(pe.szExeFile);

			if (_wcsicmp(currentProcName.c_str(), targetName.c_str()) == 0) {
				// 找到目标进程
				std::wcout << L"[MONITOR] 目标软件 (" << targetName << L") 正在运行." << std::endl;

				if (shouldBeFrozen) {
					FreezeWindowAndPrompt(targetName);
				}
				CloseHandle(hSnapshot);
				return;
			}
		} while (Process32Next(hSnapshot, &pe));
	}

	CloseHandle(hSnapshot);
}

// --------------------------- 主循环 ---------------------------

void MonitorCore::RunMonitorLoop() {
	std::wstring targetName;

	// 检查注册表数据是否存在
	if (!RegistryManager::ReadString(L"TargetName", targetName)) {
		std::wcout << L"[CORE] 错误：注册表数据不存在，请先配置。" << std::endl;
		return;
	}

	// 在核心运行前，获取并显示本地 HWID (用于客服生成密钥)
	std::wstring localHwid = HardwareID::GenerateHardwareId();
	std::wcout << L"\n[HWID] 本机硬件ID (请提供给客服):" << std::endl;
	std::wcout << localHwid << std::endl;
	std::wcout << L"--------------------------------------" << std::endl;

	// 实际服务会持续运行，这里使用一个简单的循环演示
	while (true) {
		// 1. 检查日期和状态
		bool shouldBeFrozen = CheckDatesAndState(targetName);

		// 2. 检查目标进程并执行相应操作
		ProcessMonitor(targetName, shouldBeFrozen);

		// 3. 休眠，每 5 秒检查一次 (服务中应使用更稳定的计时器或事件)
		Sleep(5000);
	}
}

// 假设我们的密钥格式是： HWID_HASH + "_" + NEW_EXPIRY_DATE + "_" + MAGIC_CODE
// 例如： A1B2C3D4..._20261231_SAFE

bool MonitorCore::ValidateKey(const std::wstring& key, const std::wstring& localHwid) {
	// 1. 密钥分解
	size_t pos1 = key.find(L"_");
	size_t pos2 = key.find(L"_", pos1 + 1);

	if (pos1 == std::wstring::npos || pos2 == std::wstring::npos) {
		std::wcout << L"[KEY] 密钥格式错误。" << std::endl;
		return false;
	}

	std::wstring receivedHwidHash = key.substr(0, pos1);
	std::wstring newExpiryDateStr = key.substr(pos1 + 1, pos2 - pos1 - 1);
	std::wstring magicCode = key.substr(pos2 + 1);

	// 2. 验证 HWID 绑定 (核心步骤)
	// 密钥中的 HWID 必须匹配当前机器的 HWID
	std::wstring expectedHwidHash = HardwareID::SHA256(localHwid);

	if (receivedHwidHash != expectedHwidHash.substr(0, receivedHwidHash.length())) {
		std::wcout << L"[KEY] 密钥不匹配此计算机！硬件 ID 验证失败。" << std::endl;
		return false;
	}

	// 3. 验证 MAGIC_CODE 和状态（永久/一次性）
	DWORD newStatus = KeyStatus::Trial;
	if (magicCode == L"FINAL") {
		newStatus = KeyStatus::PermanentActive;
	}
	else if (magicCode == L"TEMP") {
		newStatus = KeyStatus::OneTimeKeyActive;
	}
	else {
		std::wcout << L"[KEY] 密钥校验码错误。" << std::endl;
		return false;
	}

	// 4. 激活成功，更新注册表
	if (newStatus == KeyStatus::PermanentActive) {
		RegistryManager::WriteDword(L"KeyStatus", newStatus);
		std::wcout << L"[KEY] 永久密钥激活成功！监控服务已停止。" << std::endl;
	}
	else {
		RegistryManager::WriteString(L"ExpiryDate", newExpiryDateStr);
		RegistryManager::WriteDword(L"KeyStatus", newStatus);
		std::wcout << L"[KEY] 一次性密钥激活成功！到期日延长至 " << newExpiryDateStr << std::endl;
	}

	return true;
}