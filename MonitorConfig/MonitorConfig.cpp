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
		std::wcout << L"------------------------------------------------------------------" << std::endl;
		std::wcout << L"               Windows 服务部署指令 (请以管理员身份运行)  " << std::endl;
		std::wcout << L"------------------------------------------------------------------" << std::endl;

		// 假设您将 MonitorService.exe 部署在 C:\YourPath\ 目录下
		std::wcout << L"1. **安装服务 (创建服务定义):**" << std::endl;
		std::wcout << L"   sc create MonitorSvc binPath= \"C:\\YourPath\\MonitorService.exe\" start= auto obj= LocalSystem DisplayName= \"My Authorization Monitor\"" << std::endl;
		std::wcout << L"   (请将 C:\\YourPath\\MonitorService.exe 替换为实际路径，确保使用绝对路径)\n"
				   << std::endl;

		std::wcout << L"2. **设置服务自动重启 (增强抗杀性):**" << std::endl;
		std::wcout << L"   sc.exe failure MonitorSvc reset= 0 actions= restart/1000/restart/1000/restart/1000" << std::endl;
		std::wcout << L"   (服务失败后将立即重启)" << std::endl;

		std::wcout << L"3. **启动服务 (常驻监控):**" << std::endl;
		std::wcout << L"   sc.exe start MonitorSvc" << std::endl;

		std::wcout << L"4. **停止服务 (调试/维护时使用):**" << std::endl;
		std::wcout << L"   sc.exe stop MonitorSvc" << std::endl;

		std::wcout << L"5. **删除服务 (卸载时使用):**" << std::endl;
		std::wcout << L"   sc.exe delete MonitorSvc" << std::endl;

		std::wcout << L"6. **获取服务信息 (审核是否安装):**" << std::endl;
		std::wcout << L"   sc.exe delete MonitorSvc" << std::endl;

		std::wcout << L"------------------------------------------------------------------" << std::endl;
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