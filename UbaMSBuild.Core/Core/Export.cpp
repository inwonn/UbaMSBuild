#include "pch.h"
#include "Types.h"
#include "Export.h"
#include "MemoryMappedFile.h"
#include "ToolTask.h"

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
}