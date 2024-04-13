#include "pch.h"
#include "ToolTask.h"
#include "Debug.h"
#include "MemoryMappedFile.h"
#include "Types.h"

namespace uba_msbuild
{
	ToolTask::ToolTask(Segment* segment)
		: _segment(segment)
	{
	}

	ToolTask::~ToolTask()
	{
	}

	void ToolTask::Run(const nlohmann::json& json)
	{
		std::string ToolTaskStr = json.dump();
		u32_t size = static_cast<u32_t>(ToolTaskStr.size());
		setToolTask(ToolTaskStr.c_str(), size);
		DEBUG_LOG(L"sent ---> %d\n", size);

		setStatus(Created);
	}

	ToolTaskStatus ToolTask::GetResult(int timeout /*= -1*/)
	{
		ToolTaskStatus status;
		bool isDone = false;
		do
		{
			status = getStatus();
			switch (status)
			{
			case Created:
			case Canceled:
			case RanToCompletion:
			case Faulted:
			{
				isDone = true;
				break;
			}
			case Running:
				break;
			default:
				throw std::exception("Invalid ToolTaskStatus");
			}

			Sleep(1000);

			if (timeout != INFINITE)
				timeout -= 1000;

		} while (isDone == false && (timeout == INFINITE || timeout > 0));

		return status;
	}

	bool ToolTask::GetToolTask(void** outToolTask, u32_t* size)
	{
		if (getStatus() == Created)
		{
			getToolTask(outToolTask, size);
			return true;
		}

		return false;
	}

	void ToolTask::SetToolTaskStatus(ToolTaskStatus buildStatus)
	{
		setStatus(buildStatus);
	}

	ToolTaskStatus ToolTask::GetToolTaskStatus()
	{
		return getStatus();
	}

	void ToolTask::setStatus(ToolTaskStatus status)
	{
		InterlockedExchange((u32_t*)getPtr(SegmentOffset_BuildStatus), status);
	}

	void ToolTask::setToolTask(const void* message, u32_t size)
	{
		memcpy(getPtr(SegmentOffset_ToolTaskSize), &size, sizeof(u32_t));
		memcpy(getPtr(SegmentOffset_ToolTask), message, size);
	}

	ToolTaskStatus ToolTask::getStatus()
	{
		return (ToolTaskStatus)InterlockedCompareExchange((u32_t*)getPtr(SegmentOffset_BuildStatus), 0, 0);
	}

	void ToolTask::getToolTask(void** outToolTask, u32_t* size)
	{
		memcpy(size, getPtr(SegmentOffset_ToolTaskSize), sizeof(u32_t));
		memcpy(*outToolTask, getPtr(SegmentOffset_ToolTask), *size);
	}

	void* ToolTask::getPtr(SegmentOffset segmentOffset)
	{
		u32_t offset = u32_t(segmentOffset) * sizeof(u32_t);
		return _segment->data + offset;
	}
}