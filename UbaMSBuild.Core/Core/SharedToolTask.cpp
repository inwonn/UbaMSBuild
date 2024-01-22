#include "pch.h"
#include "SharedToolTask.h"
#include "Debug.h"
#include "MemoryMappedFile.h"
#include "Types.h"

#include "Generated/ToolTask.pb.h"
#include "Generated/ToolTaskStatus.pb.h"

namespace ubavs
{
	u32_t HostGetToolTaskCount(const wchar_t* buildId)
	{
		static MemoryMappedFile mappedFile(buildId);
		return mappedFile.GetCapacity();
	}

	bool HostGetToolTask(const wchar_t* buildId, int toolTaskId, void** outToolTask, u32_t* size)
	{
		static MemoryMappedFile mappedFile(buildId);

		Segment* segment = mappedFile.Get(toolTaskId);
		if (segment != nullptr && segment->IsValid())
		{
			SharedToolTask task(segment);
			return task.HostGetToolTask(outToolTask, size);
		}
		return false;
	}

	bool HostSetToolTaskStatus(const wchar_t* buildId, int toolTaskId, u32_t status)
	{
		static MemoryMappedFile mappedFile(buildId);

		Segment* segment = mappedFile.Get(toolTaskId);
		if (segment != nullptr && segment->IsValid())
		{
			SharedToolTask task(segment);
			task.HostSetToolTaskStatus((ToolTaskStatus)status);
			return true;
		}
		return false;
	}

	SharedToolTask::SharedToolTask(Segment* segment)
		: _segment(segment)
	{
	}

	SharedToolTask::~SharedToolTask()
	{
	}

	void SharedToolTask::ProviderRun(const ToolTask& toolTask)
	{
		setStatus(ProviderRunning);

		std::string ToolTaskStr = toolTask.SerializeAsString();
		u32_t size = static_cast<u32_t>(ToolTaskStr.size());
		setToolTask(ToolTaskStr.c_str(), size);
		DEBUG_LOG(L"sent ---> %d\n", size);

		setStatus(ProviderRanToCompletion);
	}

	ToolTaskStatus SharedToolTask::ProviderGetResult(int timeout /*= -1*/)
	{
		ToolTaskStatus status;
		bool isDone = false;
		do
		{
			status = getStatus();
			switch (status)
			{
			case Created:
			case Cancelled:
			case Completed:
			case Faulted:
			case HostRanToCompletion:
			case HostFaulted:
			case ProviderFaulted:
			{
				isDone = true;
				break;
			}
			case ProviderRunning:
			case ProviderRanToCompletion:
			case HostRunning:
			{
				isDone = false;
				break;
			}
			default:
				throw std::exception("Invalid ToolTaskStatus");
			}

			Sleep(1000);

			if (timeout != INFINITE)
				timeout -= 1000;

		} while (isDone == false && (timeout == INFINITE || timeout > 0));

		return status;
	}

	bool SharedToolTask::HostGetToolTask(void** outToolTask, u32_t* size)
	{
		if (getStatus() == ProviderRanToCompletion)
		{
			getToolTask(outToolTask, size);
			HostSetToolTaskStatus(HostRunning);
			return true;
		}

		return false;
	}

	void SharedToolTask::HostSetToolTaskStatus(ToolTaskStatus buildStatus)
	{
		setStatus(buildStatus);
	}

	void SharedToolTask::setStatus(ToolTaskStatus status)
	{
		InterlockedExchange((u32_t*)getPtr(SegmentOffset_BuildStatus), status);
	}

	void SharedToolTask::setToolTask(const void* message, u32_t size)
	{
		memcpy(getPtr(SegmentOffset_ToolTaskSize), &size, sizeof(u32_t));
		memcpy(getPtr(SegmentOffset_ToolTask), message, size);
	}

	ToolTaskStatus SharedToolTask::getStatus()
	{
		return (ToolTaskStatus)InterlockedCompareExchange((u32_t*)getPtr(SegmentOffset_BuildStatus), 0, 0);
	}

	void SharedToolTask::getToolTask(void** outToolTask, u32_t* size)
	{
		memcpy(size, getPtr(SegmentOffset_ToolTaskSize), sizeof(u32_t));
		memcpy(*outToolTask, getPtr(SegmentOffset_ToolTask), *size);
	}

	void* SharedToolTask::getPtr(SegmentOffset segmentOffset)
	{
		u32_t offset = u32_t(segmentOffset) * sizeof(u32_t);
		return _segment->data + offset;
	}
}