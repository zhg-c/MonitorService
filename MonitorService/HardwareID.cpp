#include "HardwareID.h"
#include <windows.h>
#include <iphlpapi.h> // ��Ҫ���� iphlpapi.lib
#include <stdexcept>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "iphlpapi.lib") // VS ����ʱ�Զ�����

namespace HardwareID {

	std::wstring GetMacAddress() {
		IP_ADAPTER_INFO* pAdapterInfo = nullptr;
		IP_ADAPTER_INFO* pAdapter = nullptr;
		DWORD dwBufLen = 0;
		std::wstring macAddress = L"";

		// 1. ��һ�ε��ã���ȡ����Ļ�������С
		if (GetAdaptersInfo(pAdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
			pAdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
		}

		// 2. �ڶ��ε��ã���ȡ����
		if (pAdapterInfo && GetAdaptersInfo(pAdapterInfo, &dwBufLen) == NO_ERROR) {
			pAdapter = pAdapterInfo;
			// ��������������ͨ��ѡ���һ�����õķ���������
			while (pAdapter) {
				if (pAdapter->AddressLength == 6) {
					std::wstringstream ss;
					// ��ʽ�� MAC ��ַ��XX-XX-XX-XX-XX-XX
					for (DWORD i = 0; i < pAdapter->AddressLength; i++) {
						ss << std::hex << std::setw(2) << std::setfill(L'0')
							<< (int)pAdapter->Address[i] << (i < 5 ? L"-" : L"");
					}
					macAddress = ss.str();
					break; // �ҵ���һ����Ч�ľ��˳�
				}
				pAdapter = pAdapter->Next;
			}
		}

		if (pAdapterInfo) {
			free(pAdapterInfo);
		}

		return macAddress;
	}

	// B. ��ȡ BIOS ���к� (SMBIOS/WMI) - ʹ�� WMI ����ͨ�ã�����Ҫ COM ��ʼ��

	std::wstring GetBiosSerialNumber() {
		// ���棺WMI ��ȡ��Ҫ COM/WMI ��ʼ���������ṩһ���򻯵� WinAPI ���� (���ܲ����Ƿ���)
		// **��ɿ��ķ�����ʹ�� WMI (Windows Management Instrumentation)**

		// �� WinAPI ʾ�� (���ܷ��� BIOS UUID ���� SerialNumber)
		std::wstring serial = L"";
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			L"HARDWARE\\DESCRIPTION\\System\\SystemBios",
			0,
			KEY_READ,
			&hKey) == ERROR_SUCCESS) {

			DWORD bufferSize = 256 * sizeof(wchar_t);
			wchar_t buffer[256] = { 0 };
			DWORD type = 0;

			if (RegQueryValueEx(hKey,
				L"BiosSystemSerialNumber", // ͨ���洢�ڴ˴�
				NULL,
				&type,
				(LPBYTE)buffer,
				&bufferSize) == ERROR_SUCCESS) {
				serial = buffer;
			}
			RegCloseKey(hKey);
		}

		// �����ȡʧ�ܣ����Գ�������·����ʹ�� WMI
		if (serial.empty()) {
			serial = L"NO_BIOS_SN";
		}

		return serial;
	}

	// C. ��ϲ��������� HWID

	std::wstring GenerateHardwareId() {
		// ��϶����ʶ���γ�һ��ǿ��Ļ���ָ��
		// ��ʾ�����Լ��� CPU ID, Ӳ�����кŵȣ��������Ӹ�����

		std::wstring biosSN = GetBiosSerialNumber();
		std::wstring mac = GetMacAddress();

		// ��ʽ: BIOS_SN|MAC_ADDR (�� | �ָ������ڽ���)
		std::wstring rawId = biosSN + L"|" + mac;

		// ��ԭʼ ID ת��ΪΨһ�Ĺ�ϣֵ
		return SHA256(rawId);
	}


	// D. �򻯰� SHA-256 ��ϣ��������������ʾ�ӿڣ�ʵ��Ӧ������ʹ��רҵ�⣡��

	// ���棺�˺������ṩʵ�ʵ� SHA-256 ʵ�֣�������һ�������ַ����Ĺ�ϣ"ռλ��"
	// �����������У����滻Ϊרҵ�ļ���ʵ�֡�
	std::wstring SHA256(const std::wstring& str) {
		if (str.empty()) return L"";

		// ģ�� SHA-256 ��ϣ�����ַ����ͳ��Ƚ��м򵥵� XOR ���㣬Ȼ���ʽ���ɹ̶�����
		unsigned long long hash_value = 0xDEADBEEF;
		for (wchar_t c : str) {
			hash_value = (hash_value ^ (unsigned long long)c) * 0x01000193; // FNV-1a ��
		}

		// ��ʽ��Ϊ 64 �ַ� (SHA-256 �ĳ���) ��ģ���ϣ�ַ���
		std::wstringstream ss;
		ss << std::hex << std::setfill(L'0') << std::setw(64) << hash_value;

		// ʵ�� SHA-256 �������Ϊ 64 ��ʮ�������ַ�
		// ����Ϊ����ʾ������ʹ�ù̶����ȵ�ģ�����
		std::wstring mockHash = L"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";
		// Ϊ����ÿ�����벻ͬʱ�����ͬ�����ǻ��һ����ģ���ϣ�͹�ϣֵ
		std::wstring result = mockHash.substr(0, 50) + ss.str().substr(0, 14);

		return result;
	}

} // namespace HardwareID