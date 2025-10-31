#pragma once
#include <windows.h>
#include <string>

// ע���·����ʵ��ʹ��ʱ���������ǿ������
#define REG_KEY_PATH L"SOFTWARE\\MyCompany\\MonitorService"

namespace RegistryManager {
	// д���ַ���ֵ
	bool WriteString(const wchar_t* valueName, const std::wstring& data);

	// ��ȡ�ַ���ֵ
	bool ReadString(const wchar_t* valueName, std::wstring& data);

	// д�� DWORD (32λ����������״̬)
	bool WriteDword(const wchar_t* valueName, DWORD data);

	// ��ȡ DWORD
	bool ReadDword(const wchar_t* valueName, DWORD& data);
}