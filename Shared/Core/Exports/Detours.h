#pragma once
#include "../Core/Core.h"
#include <Windows.h>

namespace ubavs {

    extern BOOL(*True_CreateProcessW)(
        _In_opt_ LPCWSTR lpApplicationName,
        _Inout_opt_ LPWSTR lpCommandLine,
        _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ BOOL bInheritHandles,
        _In_ DWORD dwCreationFlags,
        _In_opt_ LPVOID lpEnvironment,
        _In_opt_ LPCWSTR lpCurrentDirectory,
        _In_ LPSTARTUPINFOW lpStartupInfo,
        _Out_ LPPROCESS_INFORMATION lpProcessInformation
        );

	extern "C" {
        CORE_API BOOL CreateProcessWithDllEx(
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
            LPCWSTR lpDllName);

		CORE_API HANDLE CreateProcessWithDll(
			LPWSTR lpCommandLine,
			LPVOID lpEnvironment,
			LPCWSTR lpCurrentDirectory,
			LPCWSTR lpDllName);
	}
}

