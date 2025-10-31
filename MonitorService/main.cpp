#define NOMINMAX // 防止 min/max 宏冲突

#include "MonitorCore.h"
#include "HardwareID.h"
#include <iostream>
#include <windows.h>
#include <locale>
#include <limits> // 确保引入 std::numeric_limits

void ShowHelp()
{
	std::wcout << L"\n--- 软件监控服务控制台 ---" << std::endl;
	std::wcout << L"1. 配置并启动监控" << std::endl;
	std::wcout << L"2. 激活密钥" << std::endl;
	std::wcout << L"3. 运行监控核心 (假设服务已配置)" << std::endl;
	std::wcout << L"4. 退出" << std::endl;
	std::wcout << L"--------------------------------" << std::endl;
}

void ConfigureAndStart(MonitorCore &core)
{
	std::wstring targetName, expiryDate;

	std::wcout << L"请输入要监控的软件名称 (例如: MyApp.exe): ";
	std::wcin >> targetName;

	std::wcout << L"请输入试用截止日期 (格式 YYYYMMDD，例如 20261231): ";
	std::wcin >> expiryDate;

	if (core.InitRegistryData(targetName, expiryDate)) {
		std::wcout << L"  配置成功！数据已写入注册表。" << std::endl;
		std::wcout << L"请选择选项 3 运行监控核心。" << std::endl;
	} else {
		std::wcout << L"  配置失败，请检查权限。" << std::endl;
	}
}

void ActivateKey(MonitorCore &core)
{
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
	// 设置本地化以支持中文输入和输出
	std::locale::global(std::locale(""));

	MonitorCore core;
	int choice = 0;

	while (choice != 4) {
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
			ConfigureAndStart(core);
			break;
		case 2:
			ActivateKey(core);
			break;
		case 3:
			std::wcout << L"服务核心开始运行 (按 Ctrl+C 停止)..." << std::endl;
			core.RunMonitorLoop();
			break;
		case 4:
			std::wcout << L"退出程序。" << std::endl;
			break;
		default:
			std::wcout << L"无效的选择，请重试。" << std::endl;
			break;
		}
	}

	return 0;
}