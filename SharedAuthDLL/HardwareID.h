#pragma once
#include "SharedAuthDLL.h"
#include <string>

namespace HardwareID {
    SHAREDAUTHDLL_API std::wstring GenerateHardwareId();
    SHAREDAUTHDLL_API std::wstring SHA256(const std::wstring &input);
}
