#include "pch.h"
#include "HardwareID.h"
#include <windows.h>
#include <wincrypt.h>
#include <sstream>
#include <iomanip>
#include <vector>

#pragma comment(lib, "Advapi32.lib")

namespace HardwareID {

std::wstring SHA256(const std::wstring &input)
{
    if (input.empty()) return L"";

    int utf8Len = WideCharToMultiByte(CP_UTF8, 0,
                                      input.c_str(), (int)input.size(),
                                      NULL, 0, NULL, NULL);

    if (utf8Len <= 0) return L"";

    std::string utf8(utf8Len, 0);
    WideCharToMultiByte(CP_UTF8, 0,
                        input.c_str(), (int)input.size(),
                        &utf8[0], utf8Len, NULL, NULL);

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;

    CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);
    CryptHashData(hHash, reinterpret_cast<const BYTE*>(utf8.data()), (DWORD)utf8.size(), 0);

    DWORD cbHash = 0, cbSize = sizeof(cbHash);
    CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&cbHash, &cbSize, 0);

    std::vector<BYTE> hash(cbHash);
    CryptGetHashParam(hHash, HP_HASHVAL, hash.data(), &cbHash, 0);

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    std::wostringstream ss;
    ss << std::hex << std::uppercase << std::setfill(L'0');
    for (BYTE b : hash)
        ss << std::setw(2) << (int)b;

    return ss.str();  // 64 hex
}

std::wstring GetMachineGuid()
{
    HKEY hKey;
    wchar_t buffer[256] = {0};
    DWORD size = sizeof(buffer);
    DWORD type = 0;

    if (RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Cryptography",
            0,
            KEY_READ | KEY_WOW64_64KEY,
            &hKey
        ) != ERROR_SUCCESS) {
        return L"NO_GUID";
    }

    if (RegQueryValueExW(
            hKey,
            L"MachineGuid",
            NULL,
            &type,
            (LPBYTE)buffer,
            &size
        ) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return L"NO_GUID";
    }

    RegCloseKey(hKey);
    return buffer;
}

std::wstring GenerateHardwareId()
{
    return SHA256(GetMachineGuid());
}

} // namespace HardwareID
