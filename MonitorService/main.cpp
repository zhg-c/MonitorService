#define NOMINMAX // 防止 min/max 宏冲突

#include "MonitorCore.h"
#include "HardwareID.h"
#include <iostream>
#include <windows.h>
#include <locale>
#include <limits> // 确保引入 std::numeric_limits
#include <tchar.h> // 用于 _T() 宏

// 定义服务的名称
#define SERVICE_NAME _T("MonitorSvc")
// 全局服务状态句柄
SERVICE_STATUS_HANDLE g_ServiceStatusHandle = NULL;
SERVICE_STATUS g_ServiceStatus;

// 核心监控类的全局实例 (方便在服务函数中调用)
MonitorCore g_MonitorCore;

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

/**
 * @brief 处理来自服务控制管理器的命令（例如停止服务）。
 * @param dwControl SCM 命令代码。
 */
VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
	switch (dwControl) {
	case SERVICE_CONTROL_STOP:
		// 收到停止命令
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

		// **停止核心逻辑：** // 如果 MonitorCore::StartMonitoringLoop 是无限循环，您需要在 DLL 中添加一个退出机制。
		// 假设您在 MonitorCore 中添加了 StopMonitoring 方法。
		// g_MonitorCore.StopMonitoring();

		// 结束服务
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		break;

	case SERVICE_CONTROL_INTERROGATE:
		// 收到状态查询，直接报告当前状态
		break;

	default:
		break;
	}

	// 更新服务状态
	SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
}

/**
 * @brief 服务的实际入口点，由 SCM 调用。
 * @param dwArgc 参数数量。
 * @param lpszArgv 参数列表。
 */
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// 1. 注册服务控制处理函数
	g_ServiceStatusHandle = RegisterServiceCtrlHandler(
		SERVICE_NAME,
		ServiceCtrlHandler);

	if (g_ServiceStatusHandle == NULL) {
		// 注册失败，记录错误并退出
		return;
	}

	// 2. 初始化服务状态结构
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING; // 正在启动
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP; // 接受停止命令
	g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 3000; // 等待 3 秒

	// 3. 报告正在启动
	SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

	// 4. 服务正式运行
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwCheckPoint = 0;
	SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

	// 5. **调用您的核心监控逻辑**
	// 实际应用中，您应该从注册表读取目标进程名。
	g_MonitorCore.RunMonitorLoop();

	// 当 StartMonitoringLoop 退出（例如收到停止命令），服务结束
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
}

// 服务的入口点数组
SERVICE_TABLE_ENTRY ServiceTable[] = {
	{ const_cast<LPTSTR>(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
	{ NULL, NULL }
};

int _tmain(int argc, TCHAR *argv[])
{
	// 将控制权交给服务控制调度程序
	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
		// 调度失败，可能是程序被当作普通应用运行，而不是服务
		// 此时您可以在这里添加日志或错误处理
		return GetLastError();
	}
	return 0;
}