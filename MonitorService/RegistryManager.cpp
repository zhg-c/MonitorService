#include "RegistryManager.h"
#include <iostream>

namespace RegistryManager {

	// 通用写入函数
	static bool WriteValue(const wchar_t* valueName, DWORD type, const BYTE* data, DWORD size) {
		HKEY hKey;
		LONG result = RegCreateKeyEx(
			HKEY_CURRENT_USER, REG_KEY_PATH, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL
		);

		if (result != ERROR_SUCCESS) {
			// std::wcerr << L"RegCreateKeyEx failed: " << result << std::endl;
			return false;
		}

		result = RegSetValueEx(hKey, valueName, 0, type, data, size);
		RegCloseKey(hKey);

		// if (result != ERROR_SUCCESS) {
		//     std::wcerr << L"RegSetValueEx failed: " << result << std::endl;
		// }

		return result == ERROR_SUCCESS;
	}

	// 写入字符串值
	bool WriteString(const wchar_t* valueName, const std::wstring& data) {
		DWORD size = (data.length() + 1) * sizeof(wchar_t);
		return WriteValue(valueName, REG_SZ, (const BYTE*)data.c_str(), size);
	}

	// 写入 DWORD
	bool WriteDword(const wchar_t* valueName, DWORD data) {
		return WriteValue(valueName, REG_DWORD, (const BYTE*)&data, sizeof(DWORD));
	}

	// 读取字符串值
	bool ReadString(const wchar_t* valueName, std::wstring& data) {
		HKEY hKey;
		LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS) return false;

		DWORD bufferSize = 0;
		DWORD type = 0;

		// 第一次调用：获取所需大小
		result = RegQueryValueEx(hKey, valueName, NULL, &type, NULL, &bufferSize);
		if (result != ERROR_SUCCESS || type != REG_SZ) {
			RegCloseKey(hKey);
			return false;
		}

		// 分配缓冲区并进行第二次调用
		data.resize(bufferSize / sizeof(wchar_t));
		result = RegQueryValueEx(hKey, valueName, NULL, &type, (LPBYTE)&data[0], &bufferSize);

		RegCloseKey(hKey);

		if (result == ERROR_SUCCESS) {
			size_t nullPos = data.find(L'\0');
			if (nullPos != std::wstring::npos) {
				data.resize(nullPos);
			}
			return true;
		}
		else {
			data.clear();
			return false;
		}
	}

	// 读取 DWORD
	bool ReadDword(const wchar_t* valueName, DWORD& data) {
		HKEY hKey;
		LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS) return false;

		DWORD bufferSize = sizeof(DWORD);
		DWORD type = 0;

		result = RegQueryValueEx(hKey, valueName, NULL, &type, (LPBYTE)&data, &bufferSize);
		RegCloseKey(hKey);

		return result == ERROR_SUCCESS && type == REG_DWORD;
	}

} // namespace RegistryManager