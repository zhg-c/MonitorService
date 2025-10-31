// 修复宏冲突的关键行！
#define NOMINMAX
#include "HardwareID.h"
#include "RegistryManager.h" // 包含注册表操作
#include "MonitorCore.h"
#include <iostream>
#include <string>
#include <limits>
#include <locale>

void ShowMenu()
{
	std::wcout << L"\n--- 授权助手 v1.0 ---" << std::endl;
	std::wcout << L"1. 获取本机硬件ID (HWID)" << std::endl;
	std::wcout << L"2. 输入并激活密钥" << std::endl;
	std::wcout << L"3. 退出" << std::endl;
	std::wcout << L"-----------------------" << std::endl;
	std::wcout << L"请选择操作: ";
}

void Action_GetHwid()
{
	std::wcout << L"\n--- 本机硬件ID ---" << std::endl;
	// 调用 DLL 中的函数
	std::wstring hwid = HardwareID::GenerateHardwareId();
	std::wcout << L"请复制以下ID提供给客服获取密钥：" << std::endl;
	std::wcout << hwid << std::endl;
	std::wcout << L"-----------------------" << std::endl;
}

void ActivateKey()
{
	MonitorCore core;
	std::wstring key;
	std::wcout << L"请输入您收到的密钥: ";
	std::wcin >> key;

	// 1. 获取本地 HWID (必须在验证前获取)
	std::wstring localHwid = HardwareID::GenerateHardwareId();

	// 2. 调用新的 ValidateKey 接口
	core.ValidateKey(key, localHwid);
}

int wmain()
{
	// 确保宽字符流支持中文
	std::locale::global(std::locale(""));

	int choice = 0;
	while (choice != 3) {
		ShowMenu();
		if (!(std::wcin >> choice)) {
			std::wcin.clear();
			std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
			choice = 0;
		}

		switch (choice) {
		case 1:
			Action_GetHwid();
			break;
		case 2:
			ActivateKey();
			break;
		case 3:
			break;
		default:
			std::wcout << L"无效选择。" << std::endl;
			break;
		}
	}

	return 0;
}