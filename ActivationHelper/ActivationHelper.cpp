#define NOMINMAX
#include "HardwareID.h"
#include "MonitorCore.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <locale>
#include <limits>

void ShowMenu()
{
    std::wcout << L"\n==== 授权助手 ====" << std::endl;
    std::wcout << L"1. 显示本机 HWID" << std::endl;
    std::wcout << L"2. 激活密钥" << std::endl;
    std::wcout << L"3. 退出" << std::endl;
    std::wcout << L"> ";
}

void Action_ShowHwid()
{
    std::wstring hwid = HardwareID::GenerateHardwareId();
    std::wcout << L"\n本机 HWID:" << std::endl;
    std::wcout << hwid << L"\n\n请将此 HWID 提供给客服。" << std::endl;
}

void Action_Activate()
{
    std::wstring key;
    MonitorCore core;

    std::wcout << L"\n请输入密钥: ";
    std::wcin >> key;
    std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');

    std::wstring localHwid = HardwareID::GenerateHardwareId();
	key = Utils::Decipher(key);
    if (core.ValidateKey(key, localHwid))
        std::wcout << L"\n  授权成功！" << std::endl;
    else
        std::wcout << L"\n  授权失败，请检查密钥。" << std::endl;
}

int wmain()
{
    std::locale::global(std::locale(""));

    while (true) {
        int choice;
        ShowMenu();

        if (!(std::wcin >> choice)) {
            std::wcin.clear();
            std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
            continue;
        }

        switch (choice) {
            case 1: Action_ShowHwid(); break;
            case 2: Action_Activate(); break;
            case 3: return 0;
            default: std::wcout << L"无效选择。" << std::endl;
        }
    }
}
