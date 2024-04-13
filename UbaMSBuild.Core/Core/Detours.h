#pragma once
#include "Export.h"
#include "Types.h"

namespace uba_msbuild {
    extern CreateProcessW_t True_CreateProcessW;

	struct __declspec(uuid("{a1041c70-bf7c-4e94-8f8b-bacf0f31ba9a}")) DetoursPayload
	{
		HANDLE cancelEvent = 0;
        wchar_t buildId[MAX_PATH] = { 0, };
        wchar_t detoursLib[MAX_PATH] = { 0, };
	};
    static const _GUID DetoursPayloadGuid = __uuidof(DetoursPayload);

    BOOL CreateProcessWithDllEx(
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        LPCWSTR lpBuildId,
        LPCWSTR lpDetoursLib,
        CreateProcessW_t pfCreateProcessW);
}

