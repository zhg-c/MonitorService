#define NOMINMAX // 防止 min/max 宏冲突
#include "MonitorCore.h"
#include <iostream>
#include <windows.h>
#include <locale>

void ShowHelp()
{
	std::wcout << L"\n--- 软件监控服务控制台 ---" << std::endl;
	std::wcout << L"1. 配置监控" << std::endl;
	std::wcout << L"2. 退出" << std::endl;
	std::wcout << L"--------------------------------" << std::endl;
}

void Configure(MonitorCore &core)
{
	std::wstring targetName, expiryDate;

	std::wcout << L"请输入要监控的软件名称 (例如: MyApp.exe): ";
	std::wcin >> targetName;

	std::wcout << L"请输入试用截止日期 (格式 YYYYMMDD，例如 20261231): ";
	std::wcin >> expiryDate;

	if (core.InitRegistryData(targetName, expiryDate)) {
		std::wcout << L"              配置成功！数据已写入注册表。\n\n"
				   << std::endl;
	} else {
		std::wcout << L"  配置失败，请检查权限。" << std::endl;
	}
}

int wmain()
{
	// 设置本地化以支持中文输入和输出
	std::locale::global(std::locale(""));

	MonitorCore core;
	int choice = 0;

	while (choice != 2) {
		ShowHelp();
		std::wcout << L"请选择操作: ";
		if (!(std::wcin >> choice)) {
			std::wcin.clear();
			std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
			choice = 0;
			continue;
		}

		switch (choice) {
		case 1:
			Configure(core);
			break;
		case 2:
			std::wcout << L"退出程序。" << std::endl;
			break;
		default:
			std::wcout << L"无效的选择，请重试。" << std::endl;
			break;
		}
	}

	return 0;
}