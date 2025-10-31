#include "HardwareID.h"
#include "Utils.h" // 用于获取当前日期和计算新日期
#include <iostream>
#include <string>
#include <limits>
#include <locale>

// 密钥生成器的核心程序
void GenerateKey(const std::wstring &targetHwidHash)
{
	std::wstring expiryDateStr;
	std::wstring magicCode;
	int choice = 0;

	std::wcout << L"\n--- 密钥生成模式 ---" << std::endl;
	std::wcout << L"1. 临时一次性密钥 (例如延长 30 天)" << std::endl;
	std::wcout << L"2. 最终永久密钥" << std::endl;
	std::wcout << L"请选择密钥类型: ";

	// 输入校验
	if (!(std::wcin >> choice)) {
		std::wcin.clear();
		std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
		std::wcout << L"无效输入。" << std::endl;
		return;
	}

	std::wstring newExpiryDate;

	if (choice == 1) {
		// --- 临时密钥逻辑 ---
		magicCode = L"TEMP";

		// 模拟日期计算：获取当前日期并延长 30 天
		// **警告：实际生产中需要使用专业的日期计算库**
		std::wstring todayStr = Utils::GetCurrentDateString();

		// 假设当前日期是 YYYYMMDD 格式的 long
		long currentLong = Utils::StringDateToLong(todayStr);
		long newLong = currentLong + 30; // 这是一个非常简化的模拟，不能跨月或跨年

		// 生产环境应该使用 WMI 或 C++20 chrono 库进行精确日期运算
		newExpiryDate = std::to_wstring(newLong);

		// 为了演示，我们要求手动输入一个明确的延长日期
		std::wcout << L"请输入新的到期日期 (YYYYMMDD, 例如 20261130): ";
		std::wcin >> newExpiryDate;

		std::wcout << L"生成临时密钥，有效至 " << newExpiryDate << L"..." << std::endl;

	} else if (choice == 2) {
		// --- 永久密钥逻辑 ---
		magicCode = L"FINAL";
		newExpiryDate = L"20991231"; // 一个极远的日期
		std::wcout << L"生成永久密钥..." << std::endl;

	} else {
		std::wcout << L"无效选择。" << std::endl;
		return;
	}

	// 构造最终密钥
	// 格式: [HWID哈希前缀]_[新的截止日期 YYYYMMDD]_[魔术代码]
	// 为了提高密钥的输入效率，我们只取 HWID 哈希的前 16 个字符作为前缀。
	std::wstring keyPrefix = targetHwidHash.substr(0, 16);

	std::wstring finalKey = keyPrefix + L"_" + newExpiryDate + L"_" + magicCode;

	std::wcout << L"\n==============================================" << std::endl;
	std::wcout << L"  生成的授权密钥 (请提供给客户):" << std::endl;
	std::wcout << finalKey << std::endl;
	std::wcout << L"==============================================" << std::endl;
}

int main()
{
	// 设置本地化以支持中文
	std::locale::global(std::locale(""));

	std::wstring customerHwid;

	std::wcout << L"------------------------------------------------" << std::endl;
	std::wcout << L"       授权密钥生成工具 (客服端)  " << std::endl;
	std::wcout << L"------------------------------------------------" << std::endl;
	std::wcout << L"请输入客户提供的 64 位 本机硬件ID (HWID):" << std::endl;
	std::wcout << L"> ";

	// 接受用户输入的 HWID
	if (!(std::wcin >> customerHwid) || customerHwid.length() < 60) {
		std::wcout << L"输入无效或太短。" << std::endl;
		return 1;
	}

	// 运行密钥生成逻辑
	GenerateKey(customerHwid);

	// 保持控制台开启
	std::wcout << L"\n按任意键退出..." << std::endl;
	std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
	std::wcin.get();

	return 0;
}