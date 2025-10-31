#pragma once
#include <string>

enum KeyStatus {
	Trial = 0,
	OneTimeKeyActive = 1,
	PermanentActive = 2
};

class MonitorCore {
public:
	// ��ʼ��ע������� (�ڿ���̨����ʱ����)
	bool InitRegistryData(const std::wstring& targetName, const std::wstring& expiryDate);

	// ���������ѭ��
	void RunMonitorLoop();

	// ��֤��Կ��������Ҫ���� HWID��
	bool ValidateKey(const std::wstring& key, const std::wstring& localHwid);
private:
	// ��������Ƿ񱻴۸Ļ��ѵ���
	bool CheckDatesAndState(std::wstring& targetName);

	// ���Ŀ������Ƿ������У�������״̬���д���
	void ProcessMonitor(const std::wstring& targetName, bool shouldBeFrozen);

	// ����Ŀ�괰�ڲ�������ʾ
	void FreezeWindowAndPrompt(const std::wstring& targetName);

};