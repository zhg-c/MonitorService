#pragma once
#include <string>

enum KeyStatus {
	Trial = 0,
	OneTimeKeyActive = 1,
	PermanentActive = 2
};

class MonitorCore {
public:
	// 初始化注册表数据 (在控制台配置时调用)
	bool InitRegistryData(const std::wstring& targetName, const std::wstring& expiryDate);

	// 运行主监控循环
	void RunMonitorLoop();

	// 验证密钥（现在需要本地 HWID）
	bool ValidateKey(const std::wstring& key, const std::wstring& localHwid);
private:
	// 检查日期是否被篡改或已到期
	bool CheckDatesAndState(std::wstring& targetName);

	// 检查目标进程是否在运行，并根据状态进行处理
	void ProcessMonitor(const std::wstring& targetName, bool shouldBeFrozen);

	// 冻结目标窗口并给出提示
	void FreezeWindowAndPrompt(const std::wstring& targetName);

};