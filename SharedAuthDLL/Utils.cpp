#include "pch.h"
#include "Utils.h"
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

namespace Utils {

// 获取当前日期字符串 (YYYYMMDD)
std::wstring GetCurrentDateString()
{
	// 使用 C++20 chrono 库的 modern approach (如果编译器不支持，需使用 WinAPI GetLocalTime)
	// 假设您使用的是支持 C++11/14/17 的环境，我们先用 C 风格函数或 WinAPI 封装
	SYSTEMTIME st;
	GetLocalTime(&st); // 获取本地时间

	std::wstringstream ss;
	ss.imbue(std::locale::classic());
	ss << std::setw(4) << std::setfill(L'0') << st.wYear
	   << std::setw(2) << std::setfill(L'0') << st.wMonth
	   << std::setw(2) << std::setfill(L'0') << st.wDay;

	return ss.str();
}

// 将 YYYYMMDD 格式的日期字符串转换为数字进行比较
long StringDateToLong(const std::wstring &dateStr)
{
	try {
		// 使用 std::stod 或 std::stol 进行转换
		return std::stol(dateStr);
	} catch (...) {
		return 0; // 转换失败返回 0 (代表一个极早的日期)
	}
}

// 简单的 XOR 混淆密钥 (使用 wchar_t 类型)
const wchar_t XOR_KEY = 0xAAAA;

// ----------------------------------------------------------------------
// 1. 混淆 (XOR) - 输入输出都是 std::wstring
// ----------------------------------------------------------------------

std::wstring Obfuscate(const std::wstring &data)
{
	std::wstring obscured_data = data;
	for (wchar_t &c : obscured_data) {
		c ^= XOR_KEY;
	}
	return obscured_data;
}

// ----------------------------------------------------------------------
// 2. 编码 (wchar_t Bytes to Hex String)
// ----------------------------------------------------------------------
// 将 wchar_t 数组的字节表示转换为字母数字的 Hex 字符串
// 假设 wchar_t 是 2 字节 (Windows/MSVC 默认)
std::wstring WcharBytesToHexString(const std::wstring &wbytes)
{
	// 将 wstring 视为 byte 数组
	const unsigned char *bytes = reinterpret_cast<const unsigned char *>(wbytes.c_str());
	size_t byte_length = wbytes.length() * sizeof(wchar_t);

	std::wstringstream ss;
	ss << std::hex << std::uppercase;

	for (size_t i = 0; i < byte_length; ++i) {
		ss << std::setw(2) << std::setfill(L'0') << (int)bytes[i];
	}
	return ss.str();
}

// ----------------------------------------------------------------------
// 3. 解码 (Hex String to wchar_t Bytes)
// ----------------------------------------------------------------------

std::wstring HexStringToWcharBytes(const std::wstring &hex)
{
	if (hex.length() % (sizeof(wchar_t) * 2) != 0) {
		// 确保 Hex 字符串长度是 wchar_t 字节数的两倍的整数倍 (例如 4 的倍数)
		return L"";
	}

	std::vector<unsigned char> raw_bytes;
	for (size_t i = 0; i < hex.length(); i += 2) {
		std::wstring byteString = hex.substr(i, 2);
		// 使用 wcstoul 将宽字符 Hex 转换为数值
		unsigned char byte = (unsigned char)wcstoul(byteString.c_str(), nullptr, 16);
		raw_bytes.push_back(byte);
	}

	// 将 byte 数组重新解释为 wstring
	const wchar_t *wchars = reinterpret_cast<const wchar_t *>(raw_bytes.data());
	size_t wchars_length = raw_bytes.size() / sizeof(wchar_t);

	return std::wstring(wchars, wchars_length);
}

std::wstring Cipher(const std::wstring &str)
{
	return WcharBytesToHexString(Obfuscate(str));
}

std::wstring Decipher(const std::wstring &str)
{
	return Obfuscate(HexStringToWcharBytes(str));
}
} // namespace Utils