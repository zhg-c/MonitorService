#include "HardwareID.h" // 如果需要 SHA256 可用，但此工具本身不读取硬件
#include "Utils.h"      // 用于日期校验
#include <iostream>
#include <string>
#include <limits>
#include <locale>
#include <algorithm>
#include <cctype>

// ---------------------
// 工具函数：去掉首尾空白
// ---------------------
static std::wstring Trim(const std::wstring &s)
{
	size_t a = 0;
	while (a < s.size() && iswspace(s[a])) ++a;
	size_t b = s.size();
	while (b > a && iswspace(s[b - 1])) --b;
	return s.substr(a, b - a);
}

// ---------------------
// 工具函数：转大写
// ---------------------
static void ToUpperInPlace(std::wstring &s)
{
	std::transform(s.begin(), s.end(), s.begin(), towupper);
}

// ---------------------
// 工具函数：检查是否是 HEX
// ---------------------
static bool IsHexString(const std::wstring &s)
{
	for (wchar_t c : s) {
		if (!(iswdigit(c) || (c >= L'A' && c <= L'F') || (c >= L'a' && c <= L'f')))
			return false;
	}
	return true;
}

// ---------------------
// 核心：生成密钥
// ---------------------
void GenerateKey(const std::wstring &localHwid)
{
	std::wstring fullHwidHash = HardwareID::SHA256(localHwid);
	// --------- 1) 基本 HWID 校验 ----------
	std::wstring hwid = Trim(fullHwidHash);
	ToUpperInPlace(hwid);

	if (hwid.size() != 64) {
		std::wcout << L"[ERROR] 客户提供的 HWID 长度必须为 64 HEX 字符。" << std::endl;
		return;
	}
	if (!IsHexString(hwid)) {
		std::wcout << L"[ERROR] HWID 必须是十六进制字符串 (0-9, A-F)。" << std::endl;
		return;
	}

	// 前缀使用前 16 HEX（安全长度）
	std::wstring keyPrefix = hwid.substr(0, 16);

	// --------- 2) 选择类型 ----------
	int choice = 0;
	std::wcout << L"\n--- 密钥生成模式 ---" << std::endl;
	std::wcout << L"1. 临时一次性密钥 (自定义到期日期)" << std::endl;
	std::wcout << L"2. 最终永久密钥" << std::endl;
	std::wcout << L"请选择 (1/2): ";

	if (!(std::wcin >> choice)) {
		std::wcout << L"[ERROR] 输入无效。" << std::endl;
		return;
	}

	std::wstring newExpiryDate;
	std::wstring magicCode;

	if (choice == 1) {
		magicCode = L"TEMP";

		std::wcout << L"请输入新的到期日期 (YYYYMMDD): ";
		std::wcin >> newExpiryDate;

		if (newExpiryDate.size() != 8 || !Utils::StringDateToLong(newExpiryDate)) {
			std::wcout << L"[ERROR] 日期格式错误，应为 YYYYMMDD。" << std::endl;
			return;
		}

	} else if (choice == 2) {
		magicCode = L"FINAL";
		newExpiryDate = L"20991231";
	} else {
		std::wcout << L"[ERROR] 选择无效。" << std::endl;
		return;
	}

	// --------- 3) 生成密钥 ----------
	std::wstring finalKey =
		keyPrefix + L"_" + newExpiryDate + L"_" + magicCode;

	// --------- 4) 输出 ----------
	std::wcout << L"\n==============================================" << std::endl;
	std::wcout << L"   生成的授权密钥：" << std::endl;
	std::wcout << L" " << finalKey << std::endl;
	std::wcout << L"==============================================" << std::endl;
}

int main()
{
	std::locale::global(std::locale(""));

	std::wstring customerHwid;

	std::wcout << L"------------------------------------------------" << std::endl;
	std::wcout << L"       授权密钥生成工具 (供应商端)" << std::endl;
	std::wcout << L"------------------------------------------------" << std::endl;
	std::wcout << L"请输入客户提供的 64 位 本机硬件ID (HWID):" << std::endl;
	std::wcout << L"> ";

	if (!(std::wcin >> customerHwid)) {
		std::wcout << L"[ERROR] 输入错误。" << std::endl;
		return 1;
	}

	GenerateKey(customerHwid);

	std::wcout << L"\n按回车退出..." << std::endl;
	std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
	std::wcin.get();

	return 0;
}
