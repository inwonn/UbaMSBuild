#pragma once

#ifdef _DEBUG
namespace uba_msbuild {
	void OutputDebugStringFormat(const wchar_t* file, int line, const wchar_t* format, ...);
}

#define DEBUG_LOG(...) uba_msbuild::OutputDebugStringFormat(__FILEW__, __LINE__, __VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif


