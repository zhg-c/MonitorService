#include "Utils.h"
#include <Windows.h>
#include <sstream>
#include <iomanip>

namespace Utils {

	// 获取当前日期字符串 (YYYYMMDD)
	std::wstring GetCurrentDateString() {
		// 使用 C++20 chrono 库的 modern approach (如果编译器不支持，需使用 WinAPI GetLocalTime)
		// 假设您使用的是支持 C++11/14/17 的环境，我们先用 C 风格函数或 WinAPI 封装
		SYSTEMTIME st;
		GetLocalTime(&st); // 获取本地时间

		std::wstringstream ss;
		ss << std::setw(4) << std::setfill(L'0') << st.wYear
			<< std::setw(2) << std::setfill(L'0') << st.wMonth
			<< std::setw(2) << std::setfill(L'0') << st.wDay;

		return ss.str();
	}

	// 将 YYYYMMDD 格式的日期字符串转换为数字进行比较
	long StringDateToLong(const std::wstring& dateStr) {
		try {
			// 使用 std::stod 或 std::stol 进行转换
			return std::stol(dateStr);
		}
		catch (...) {
			return 0; // 转换失败返回 0 (代表一个极早的日期)
		}
	}

} // namespace Utils