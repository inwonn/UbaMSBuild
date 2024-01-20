#include "pch.h"
#include "BuildTask.h"
#include "Debug.h"
#include "MemoryMappedFile.h"
#include "Types.h"
#include "Generated/BuildMessage.pb.h"

namespace ubavs
{
	u32_t HostGetBuildTaskCount(const wchar_t* buildId)
	{
		static MemoryMappedFile mappedFile(buildId);
		return mappedFile.GetCapacity();
	}

	bool HostGetBuildMessage(const wchar_t* buildId, int buildTaskId, void** outMessage, u32_t* size)
	{
		static MemoryMappedFile mappedFile(buildId);

		Segment* segment = mappedFile.Get(buildTaskId);
		if (segment != nullptr && segment->IsValid())
		{
			BuildTask task(segment);
			return task.HostGetBuildMessage(outMessage, size);
		}
		return false;
	}

	bool HostSetBuildTaskStatus(const wchar_t* buildId, int buildTaskId, u32_t status)
	{
		static MemoryMappedFile mappedFile(buildId);

		Segment* segment = mappedFile.Get(buildTaskId);
		if (segment != nullptr && segment->IsValid())
		{
			BuildTask task(segment);
			task.HostSetBuildStatus((BuildTaskStatus)status);
			return true;
		}
		return false;
	}

	BuildTask::BuildTask(Segment* segment)
		: _segment(segment)
	{
	}

	BuildTask::~BuildTask()
	{
	}

	void BuildTask::ProviderRun(const BuildMessage& buildMessage)
	{
		setStatus(BuildTaskStatus_ProviderRunning);

		std::string buildMessageStr = buildMessage.SerializeAsString();
		u32_t size = static_cast<u32_t>(buildMessageStr.size());
		setBuildMessage(buildMessageStr.c_str(), size);
		DEBUG_LOG(L"sent ---> %d\n", size);

		setStatus(BuildTaskStatus_ProviderRanToCompletion);
	}

	BuildTaskStatus BuildTask::ProviderGetResult(int timeout /*= -1*/)
	{
		BuildTaskStatus status;
		bool isDone = false;
		do
		{
			status = getStatus();
			switch (status)
			{
			case BuildTaskStatus_Created:
			case BuildTaskStatus_Cancelled:
			case BuildTaskStatus_Completed:
			case BuildTaskStatus_Faulted:
			case BuildTaskStatus_HostRanToCompletion:
			case BuildTaskStatus_HostFaulted:
			case BuildTaskStatus_ProviderFaulted:
			{
				isDone = true;
				break;
			}
			case BuildTaskStatus_ProviderRunning:
			case BuildTaskStatus_ProviderRanToCompletion:
			case BuildTaskStatus_HostRunning:
			{
				isDone = false;
				break;
			}
			default:
				throw std::exception("Invalid BuildTaskStatus");
			}

			Sleep(1000);

			if (timeout != INFINITE)
				timeout -= 1000;

		} while (isDone == false && (timeout == INFINITE || timeout > 0));

		return status;
	}

	bool BuildTask::HostGetBuildMessage(void** outMessage, u32_t* size)
	{
		if (getStatus() == BuildTaskStatus_ProviderRanToCompletion)
		{
			getBuildMessage(outMessage, size);
			HostSetBuildStatus(BuildTaskStatus_HostRunning);
			return true;
		}

		return false;
	}

	void BuildTask::HostSetBuildStatus(BuildTaskStatus buildStatus)
	{
		setStatus(buildStatus);
	}

	void BuildTask::setStatus(BuildTaskStatus status)
	{
		InterlockedExchange((u32_t*)getPtr(SegmentOffset_BuildStatus), status);
	}

	void BuildTask::setBuildMessage(const void* message, u32_t size)
	{
		memcpy(getPtr(SegmentOffset_BuildMessageSize), &size, sizeof(u32_t));
		memcpy(getPtr(SegmentOffset_BuildMessage), message, size);
	}

	BuildTaskStatus BuildTask::getStatus()
	{
		return (BuildTaskStatus)InterlockedCompareExchange((u32_t*)getPtr(SegmentOffset_BuildStatus), 0, 0);
	}

	void BuildTask::getBuildMessage(void** outMessage, u32_t* size)
	{
		memcpy(size, getPtr(SegmentOffset_BuildMessageSize), sizeof(u32_t));
		memcpy(*outMessage, getPtr(SegmentOffset_BuildMessage), *size);
	}

	void* BuildTask::getPtr(SegmentOffset segmentOffset)
	{
		u32_t offset = u32_t(segmentOffset) * sizeof(u32_t);
		return _segment->data + offset;
	}
}