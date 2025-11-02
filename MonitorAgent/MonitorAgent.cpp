#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <algorithm>

// 返回与给定可执行文件名匹配的所有进程 ID（不区分大小写）
std::vector<DWORD> GetProcessIdsByName(const std::wstring &name)
{
	std::vector<DWORD> result;
	PROCESSENTRY32 pe { sizeof(pe) };

	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE)
		return result;

	if (Process32First(snap, &pe)) {
		do {
			// pe.szExeFile 是宽字符 (TCHAR) ，与 name 比较使用 _wcsicmp 忽略大小写
			if (_wcsicmp(pe.szExeFile, name.c_str()) == 0) {
				result.push_back(pe.th32ProcessID);
			}
		} while (Process32Next(snap, &pe));
	}

	CloseHandle(snap);
	return result;
}

// 判断主窗口（与之前相同）
BOOL IsMainWindow(HWND h)
{
	if (!IsWindowVisible(h))
		return FALSE;
	if (GetWindow(h, GW_OWNER) != NULL)
		return FALSE;
	LONG ex = GetWindowLong(h, GWL_EXSTYLE);
	if (ex & WS_EX_TOOLWINDOW)
		return FALSE;
	return TRUE;
}

// 回调数据：每次枚举某个 PID 时，保存该 PID 的所有主窗口
struct EnumWindowsCallbackData {
	DWORD targetPid;
	std::vector<HWND> windows;
};

BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lp)
{
	EnumWindowsCallbackData *data = reinterpret_cast<EnumWindowsCallbackData *>(lp);
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);

	if (pid == data->targetPid && IsMainWindow(hWnd)) {
		data->windows.push_back(hWnd);
	}
	return TRUE; // 继续枚举，收集所有目标窗口
}

// 示例：对指定进程名的每个实例，冻结其所有主窗口并（可选）弹窗
void FreezeAllInstancesByName(const std::wstring &procName)
{
	// 1) 找到所有 PID
	std::vector<DWORD> pids = GetProcessIdsByName(procName);
	if (pids.empty())
		return;

	// 2) 对每个 PID 枚举并收集窗口
	for (DWORD pid : pids) {
		EnumWindowsCallbackData cb {};
		cb.targetPid = pid;
		EnumWindows(EnumProc, reinterpret_cast<LPARAM>(&cb));

		// 3) 冻结该 PID 的所有主窗口
		for (HWND hWnd : cb.windows) {
			if (!IsWindow(hWnd))
				continue;
			// 跳过已冻结的窗口
			if (!IsWindowEnabled(hWnd))
				continue;

			EnableWindow(hWnd, FALSE);

			MessageBox(hWnd,
				L"试用期已到或检测到系统时间异常，请联系支持。",
				L"授权警告",
				MB_ICONSTOP | MB_OK);
		}
	}
}
// ✅ Windows GUI 程序入口点 
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR cmdLine, int) { 
	int argc = 0; 
	LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);
	if (!argv || argc < 1) 
		return 0; 
	// ✅ 进程名
	std::wstring targetName = argv[0]; 
	LocalFree(argv);
	FreezeAllInstancesByName(targetName);
	return 0;
}