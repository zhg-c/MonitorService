#pragma once
#pragma once
#include <string>
#include <chrono>

namespace Utils {
	// ��ȡ��ǰ�����ַ��� (YYYYMMDD)
	std::wstring GetCurrentDateString();

	// �� YYYYMMDD ��ʽ�������ַ���ת��Ϊ���ֽ��бȽ�
	long StringDateToLong(const std::wstring& dateStr);
}