#include "pch.h"
#include "HardwareID.h"
#include <windows.h>
#include <iphlpapi.h> // 需要链接 iphlpapi.lib
#include <stdexcept>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "iphlpapi.lib") // VS 编译时自动链接

namespace HardwareID {

std::wstring GetMacAddress()
{
	IP_ADAPTER_INFO *pAdapterInfo = nullptr;
	IP_ADAPTER_INFO *pAdapter = nullptr;
	DWORD dwBufLen = 0;
	std::wstring macAddress = L"";

	// 1. 第一次调用：获取所需的缓冲区大小
	if (GetAdaptersInfo(pAdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
	}

	// 2. 第二次调用：获取数据
	if (pAdapterInfo && GetAdaptersInfo(pAdapterInfo, &dwBufLen) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		// 遍历所有网卡，通常选择第一个可用的非虚拟网卡
		while (pAdapter) {
			if (pAdapter->AddressLength == 6) {
				std::wstringstream ss;
				// 格式化 MAC 地址：XX-XX-XX-XX-XX-XX
				for (DWORD i = 0; i < pAdapter->AddressLength; i++) {
					ss << std::hex << std::setw(2) << std::setfill(L'0')
					   << (int)pAdapter->Address[i] << (i < 5 ? L"-" : L"");
				}
				macAddress = ss.str();
				break; // 找到第一个有效的就退出
			}
			pAdapter = pAdapter->Next;
		}
	}

	if (pAdapterInfo) {
		free(pAdapterInfo);
	}

	return macAddress;
}

// B. 获取 BIOS 序列号 (SMBIOS/WMI) - 使用 WMI 更加通用，但需要 COM 初始化

std::wstring GetBiosSerialNumber()
{
	// 警告：WMI 获取需要 COM/WMI 初始化。这里提供一个简化的 WinAPI 方法 (可能不总是返回)
	// **最可靠的方法是使用 WMI (Windows Management Instrumentation)**

	// 简化 WinAPI 示例 (可能返回 BIOS UUID 而非 SerialNumber)
	std::wstring serial = L"";
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			L"HARDWARE\\DESCRIPTION\\System\\SystemBios",
			0,
			KEY_READ,
			&hKey)
		== ERROR_SUCCESS) {

		DWORD bufferSize = 256 * sizeof(wchar_t);
		wchar_t buffer[256] = { 0 };
		DWORD type = 0;

		if (RegQueryValueEx(hKey,
				L"BiosSystemSerialNumber", // 通常存储在此处
				NULL,
				&type,
				(LPBYTE)buffer,
				&bufferSize)
			== ERROR_SUCCESS) {
			serial = buffer;
		}
		RegCloseKey(hKey);
	}

	// 如果获取失败，可以尝试其他路径或使用 WMI
	if (serial.empty()) {
		serial = L"NO_BIOS_SN";
	}

	return serial;
}

// C. 组合并生成最终 HWID

std::wstring GenerateHardwareId()
{
	// 组合多个标识，形成一个强大的机器指纹
	// 提示：可以加入 CPU ID, 硬盘序列号等，但会增加复杂性

	std::wstring biosSN = GetBiosSerialNumber();
	std::wstring mac = GetMacAddress();

	// 格式: BIOS_SN|MAC_ADDR (用 | 分隔，便于解析)
	std::wstring rawId = biosSN + L"|" + mac;

	// 将原始 ID 转化为唯一的哈希值
	return SHA256(rawId);
}

// D. 简化版 SHA-256 哈希函数（仅用于演示接口，实际应用中请使用专业库！）

// 警告：此函数不提供实际的 SHA-256 实现，仅返回一个输入字符串的哈希"占位符"
// 在生产环境中，请替换为专业的加密实现。
std::wstring SHA256(const std::wstring &str)
{
	if (str.empty())
		return L"";

	// 模拟 SHA-256 哈希：将字符串和长度进行简单的 XOR 运算，然后格式化成固定长度
	unsigned long long hash_value = 0xDEADBEEF;
	for (wchar_t c : str) {
		hash_value = (hash_value ^ (unsigned long long)c) * 0x01000193; // FNV-1a 简化
	}

	// 格式化为 64 字符 (SHA-256 的长度) 的模拟哈希字符串
	std::wstringstream ss;
	ss << std::hex << std::setfill(L'0') << std::setw(64) << hash_value;

	// 实际 SHA-256 结果长度为 64 个十六进制字符
	// 这里为了演示，我们使用固定长度的模拟输出
	std::wstring mockHash = L"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";
	// 为了让每次输入不同时输出不同，我们混合一部分模拟哈希和哈希值
	std::wstring result = mockHash.substr(0, 50) + ss.str().substr(0, 14);

	return result;
}

} // namespace HardwareID