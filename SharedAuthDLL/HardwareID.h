#pragma once
#include "SharedAuthDLL.h"
#include <string>
namespace HardwareID {
// ----------------------------------------------------------------
// 硬件获取函数
// ----------------------------------------------------------------

// 获取主板 BIOS 序列号 (通常是比较稳定的标识)
SHAREDAUTHDLL_API std::wstring GetBiosSerialNumber();

// 获取第一个有效的 MAC 地址 (需链接 iphlpapi.lib)
SHAREDAUTHDLL_API std::wstring GetMacAddress();

// 组合多个标识，生成最终的硬件ID字符串
SHAREDAUTHDLL_API std::wstring GenerateHardwareId();

// ----------------------------------------------------------------
// 简化版 SHA-256 哈希函数 (将任意长字符串转换为固定 64 字符的哈希值)
// ----------------------------------------------------------------
SHAREDAUTHDLL_API std::wstring SHA256(const std::wstring &str);
}