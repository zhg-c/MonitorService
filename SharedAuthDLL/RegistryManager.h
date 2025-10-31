#pragma once
#include "SharedAuthDLL.h"
#include <windows.h>
#include <string>

// 注册表路径，实际使用时请更改以增强隐蔽性
#define REG_KEY_PATH L"SOFTWARE\\MyCompany\\MonitorService"

namespace RegistryManager {
// 写入字符串值
SHAREDAUTHDLL_API bool WriteString(const wchar_t *valueName, const std::wstring &data);

// 读取字符串值
SHAREDAUTHDLL_API bool ReadString(const wchar_t *valueName, std::wstring &data);

// 写入 DWORD (32位整数，用于状态)
SHAREDAUTHDLL_API bool WriteDword(const wchar_t *valueName, DWORD data);

// 读取 DWORD
SHAREDAUTHDLL_API bool ReadDword(const wchar_t *valueName, DWORD &data);
}