#pragma once
#include "Export.h"
#include "Types.h"

namespace ubavs {

	class Segment;
	class ToolTask;
	enum ToolTaskStatus;

	enum SegmentOffset : u32_t
	{
		SegmentOffset_BuildStatus = 0,
		SegmentOffset_ToolTaskSize = 1,
		SegmentOffset_ToolTask = 2,
	};

	class SharedToolTask
	{
	public:
		SharedToolTask(Segment* segment);
		~SharedToolTask();

		void ProviderRun(const ToolTask& ToolTask);
		ToolTaskStatus ProviderGetResult(int timeout = INFINITE);

		bool HostGetToolTask(void** outToolTask, u32_t* size);
		void HostSetToolTaskStatus(ToolTaskStatus buildStatus);

	private:

		void setStatus(ToolTaskStatus status);
		void setToolTask(const void* message, u32_t size);

		ToolTaskStatus getStatus();
		void getToolTask(void** outToolTask, u32_t* size);

		void* getPtr(SegmentOffset offset);
		

	private:
		Segment* _segment;
	};

	extern "C"
	{
		CORE_API u32_t HostGetToolTaskCount(const wchar_t* buildId);
		CORE_API bool HostGetToolTask(const wchar_t* buildId, int toolTaskId, void** outToolTask, u32_t* size);
		CORE_API bool HostSetToolTaskStatus(const wchar_t* buildId, int toolTaskId, u32_t status);
	} 
}