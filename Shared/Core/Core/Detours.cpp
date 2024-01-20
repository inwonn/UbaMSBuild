#include "pch.h"
#include "Detours.h"

#include <vector>
#include <string>
#include <boost/scope_exit.hpp>
#include <boost/locale.hpp>
#include <detours/detours.h>

namespace ubavs {

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

	HANDLE CreateProcessWithDll(
		LPWSTR lpCommandLine,
		LPCWSTR lpBuildId,
		LPCWSTR lpDetoursLib)
	{
		STARTUPINFOEX siex;
		STARTUPINFO& si = siex.StartupInfo;
		ZeroMemory(&siex, sizeof(STARTUPINFOEX));
		si.cb = sizeof(STARTUPINFOEX);

		PROCESS_INFORMATION processInfo;
		ZeroMemory(&processInfo, sizeof(processInfo));

		DWORD creationFlags = CREATE_DEFAULT_ERROR_MODE |  CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW;
		BOOL inheritHandles = false;

		SIZE_T attributesBufferSize = 0;
		::InitializeProcThreadAttributeList(nullptr, 1, 0, &attributesBufferSize);

		std::vector<u8_t> attributesBuffer(attributesBufferSize, 0);

		PPROC_THREAD_ATTRIBUTE_LIST attributes = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(attributesBuffer.data());
		if (!::InitializeProcThreadAttributeList(attributes, 1, 0, &attributesBufferSize))
		{
			//logger.Error(TC("InitializeProcThreadAttributeList failed (%s)"), LastErrorToText().data);
			return INVALID_HANDLE_VALUE;
		}

		BOOST_SCOPE_EXIT(&attributes) {
			::DeleteProcThreadAttributeList(attributes);
		} BOOST_SCOPE_EXIT_END


		siex.lpAttributeList = attributes;
		creationFlags |= EXTENDED_STARTUPINFO_PRESENT;

		/*ScopedReadLock jobObjectLock(m_session.m_processJobObjectLock);
		if (!m_session.m_processJobObject)
		{
			m_cancelEvent.Set();
			return ProcessCancelExitCode;
		}*/

		HANDLE hJob = CreateJobObject(nullptr, nullptr);
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { };
		info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_BREAKAWAY_OK;
		SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &info, sizeof(info));

		//HANDLE jobs[] = { m_session.m_processJobObject, m_accountingJobObject };
		HANDLE jobs[] = { hJob };

		if (!::UpdateProcThreadAttribute(attributes, 0, PROC_THREAD_ATTRIBUTE_JOB_LIST, jobs, sizeof(jobs), nullptr, nullptr))
		{
			//logger.Error(TC("UpdateProcThreadAttribute failed when setting job list (%s)"), LastErrorToText().data);
			return INVALID_HANDLE_VALUE;
		}

		if (!CreateProcessWithDllEx(NULL, lpCommandLine, NULL, NULL, inheritHandles, creationFlags, NULL, NULL, &si, &processInfo, lpBuildId, lpDetoursLib, True_CreateProcessW))
			return INVALID_HANDLE_VALUE;

		return processInfo.hProcess;
	}
}

