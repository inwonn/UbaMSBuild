#include "pch.h"
#include "Detours.h"

#include <vector>
#include <string>
#include <boost/locale.hpp>
#include <detours/detours.h>

namespace uba_msbuild {

	CreateProcessW_t True_CreateProcessW = ::CreateProcessW;

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
		CreateProcessW_t pfCreateProcessW)
	{
		std::string detoursLib = boost::locale::conv::utf_to_utf<char>(lpDetoursLib);
		if (!DetourCreateProcessWithDll(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, detoursLib.c_str(), pfCreateProcessW))
		{
			return FALSE;
		}

		DetoursPayload payload;
		wcscpy_s(payload.buildId, wcslen(lpBuildId) * sizeof(wchar_t), lpBuildId);
		wcscpy_s(payload.detoursLib, wcslen(lpDetoursLib) * sizeof(wchar_t), lpDetoursLib);
		if (!DetourCopyPayloadToProcess((HANDLE)lpProcessInformation->hProcess, DetoursPayloadGuid, &payload, sizeof(payload)))
		{
			return FALSE;
		}

		//if (!AlternateGroupAffinity(lpProcessInformation->hThread))
		//{
		//	//logger.Error(TC("Failed to set thread group affinity to process"));//% ls. (% ls)"), commandLine.c_str(), LastErrorToText().data);
		//	return FALSE;
		//}

		if (ResumeThread(lpProcessInformation->hThread) == -1)
		{
			//logger.Error(TC("Failed to resume thread for"));//% ls. (% ls)", commandLine.c_str(), LastErrorToText().data);
			return FALSE;
		}

		return TRUE;
	}
}

