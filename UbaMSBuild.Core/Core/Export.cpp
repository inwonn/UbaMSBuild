#include "pch.h""
#include "Debug.h"
#include "Detours.h"
#include "Export.h"
#include "Types.h"
#include "MemoryMappedFile.h"
#include "ToolTask.h"

#include <boost/scope_exit.hpp>
#include <boost/locale.hpp>

namespace uba_msbuild
{
	u32_t GetToolTaskCount(const wchar_t* buildId)
	{
		static MemoryMappedFile mappedFile(buildId);
		return mappedFile.GetCapacity();
	}

	bool GetToolTask(const wchar_t* buildId, int toolTaskId, void** outToolTask, u32_t* size)
	{
		static MemoryMappedFile mappedFile(buildId);

		Segment* segment = mappedFile.Get(toolTaskId);
		if (segment != nullptr && segment->IsValid())
		{
			ToolTask task(segment);
			return task.GetToolTask(outToolTask, size);
		}
		return false;
	}

	bool SetToolTaskStatus(const wchar_t* buildId, int toolTaskId, u32_t status)
	{
		static MemoryMappedFile mappedFile(buildId);

		Segment* segment = mappedFile.Get(toolTaskId);
		if (segment != nullptr && segment->IsValid())
		{
			ToolTask task(segment);
			task.SetToolTaskStatus((ToolTaskStatus)status);
			return true;
		}
		return false;
	}

	CORE_API u32_t GetToolTaskStatus(const wchar_t* buildId, int toolTaskId)
	{
		static MemoryMappedFile mappedFile(buildId);

		Segment* segment = mappedFile.Get(toolTaskId);
		if (segment != nullptr && segment->IsValid())
		{
			ToolTask task(segment);
			return task.GetToolTaskStatus();
		}
		return 0xffffffff;
	}

	HANDLE CreateProcessWithDll(
		LPWSTR lpCommandLine,
		LPCWSTR lpBuildId,
		LPCWSTR lpDetoursLib,
		LPDWORD lpProcessId)
	{
		STARTUPINFOEX siex;
		STARTUPINFO& si = siex.StartupInfo;
		ZeroMemory(&siex, sizeof(STARTUPINFOEX));
		si.cb = sizeof(STARTUPINFOEX);

		PROCESS_INFORMATION processInfo;
		ZeroMemory(&processInfo, sizeof(processInfo));

		DWORD creationFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW;
		BOOL inheritHandles = false;

		SIZE_T attributesBufferSize = 0;
		::InitializeProcThreadAttributeList(nullptr, 1, 0, &attributesBufferSize);

		std::vector<u8_t> attributesBuffer(attributesBufferSize, 0);

		PPROC_THREAD_ATTRIBUTE_LIST attributes = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(attributesBuffer.data());
		if (!::InitializeProcThreadAttributeList(attributes, 1, 0, &attributesBufferSize))
		{
			DEBUG_LOG(L"InitializeProcThreadAttributeList failed (%ld)\n", GetLastError());
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
			DEBUG_LOG(L"UpdateProcThreadAttribute failed when setting job list (%ld)", GetLastError());
			return INVALID_HANDLE_VALUE;
		}

		if (!CreateProcessWithDllEx(NULL, lpCommandLine, NULL, NULL, inheritHandles, creationFlags, NULL, NULL, &si, &processInfo, lpBuildId, lpDetoursLib, True_CreateProcessW))
		{
			DEBUG_LOG(L"CreateProcessWithDllEx failed (%ld)", GetLastError());
			return INVALID_HANDLE_VALUE;
		}

		if (lpProcessId)
		{
			*lpProcessId = processInfo.dwProcessId;
		}

		return processInfo.hProcess;
	}
}