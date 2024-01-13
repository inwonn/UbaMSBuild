#pragma once

#ifdef _DEBUG
namespace ubavs {
	void OutputDebugStringFormat(const wchar_t* format, const wchar_t* file, const wchar_t* function, int line, ...);
}

#define DEBUG_LOG(format, ...) ubavs::OutputDebugStringFormat(format, __FILEW__, __FUNCTIONW__, __LINE__, __VA_ARGS__)
#else
#define DEBUG_LOG(format, ...)
#endif


