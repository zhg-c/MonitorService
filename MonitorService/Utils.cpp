#include "Utils.h"
#include <Windows.h>
#include <sstream>
#include <iomanip>

namespace Utils {

	// ��ȡ��ǰ�����ַ��� (YYYYMMDD)
	std::wstring GetCurrentDateString() {
		// ʹ�� C++20 chrono ��� modern approach (�����������֧�֣���ʹ�� WinAPI GetLocalTime)
		// ������ʹ�õ���֧�� C++11/14/17 �Ļ������������� C ������� WinAPI ��װ
		SYSTEMTIME st;
		GetLocalTime(&st); // ��ȡ����ʱ��

		std::wstringstream ss;
		ss << std::setw(4) << std::setfill(L'0') << st.wYear
			<< std::setw(2) << std::setfill(L'0') << st.wMonth
			<< std::setw(2) << std::setfill(L'0') << st.wDay;

		return ss.str();
	}

	// �� YYYYMMDD ��ʽ�������ַ���ת��Ϊ���ֽ��бȽ�
	long StringDateToLong(const std::wstring& dateStr) {
		try {
			// ʹ�� std::stod �� std::stol ����ת��
			return std::stol(dateStr);
		}
		catch (...) {
			return 0; // ת��ʧ�ܷ��� 0 (����һ�����������)
		}
	}

} // namespace Utils