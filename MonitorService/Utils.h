#pragma once
#pragma once
#include <string>
#include <chrono>

namespace Utils {
	// 获取当前日期字符串 (YYYYMMDD)
	std::wstring GetCurrentDateString();

	// 将 YYYYMMDD 格式的日期字符串转换为数字进行比较
	long StringDateToLong(const std::wstring& dateStr);
}