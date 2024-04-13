#pragma once
#include "Types.h"

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

namespace uba_msbuild
{
	extern "C"
	{
		CORE_API u32_t GetToolTaskCount(const wchar_t* buildId);
		CORE_API bool GetToolTask(const wchar_t* buildId, int toolTaskId, void** outToolTask, u32_t* size);
		CORE_API bool SetToolTaskStatus(const wchar_t* buildId, int toolTaskId, u32_t status);

		CORE_API HANDLE CreateProcessWithDll(
			LPWSTR lpCommandLine,
			LPCWSTR lpBuildId,
			LPCWSTR lpDetoursLib);
	}
}