#pragma once
#include "SharedAuthDLL.h"
#include <string>
#include <chrono>

namespace Utils {
// 获取当前日期字符串 (YYYYMMDD)
SHAREDAUTHDLL_API std::wstring GetCurrentDateString();

// 将 YYYYMMDD 格式的日期字符串转换为数字进行比较
SHAREDAUTHDLL_API long StringDateToLong(const std::wstring &dateStr);
}