#pragma once
#pragma once

// 定义动态库导出/导入宏
#ifdef SHAREDAUTHDLL_EXPORTS
#define SHAREDAUTHDLL_API __declspec(dllexport)
#else
#define SHAREDAUTHDLL_API __declspec(dllimport)
#endif