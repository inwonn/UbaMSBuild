#pragma once
#include "Export.h"
#include "Types.h"

namespace ubavs {

	class BuildMessage;
	class Segment;

	enum BuildTaskStatus : u32_t
	{
		BuildTaskStatus_Created = 0,
		BuildTaskStatus_Cancelled = 1,
		BuildTaskStatus_Completed = 2,
		BuildTaskStatus_Faulted = 3,
		BuildTaskStatus_ProviderRunning = 4,
		BuildTaskStatus_ProviderRanToCompletion = 5,
		BuildTaskStatus_ProviderFaulted = 6,
		BuildTaskStatus_HostRunning = 7,
		BuildTaskStatus_HostRanToCompletion = 8,
		BuildTaskStatus_HostFaulted = 9,
	};

	enum SegmentOffset : u32_t
	{
		SegmentOffset_BuildStatus = 0,
		SegmentOffset_BuildMessageSize = 1,
		SegmentOffset_BuildMessage = 2,
	};

	class BuildTask
	{
	public:
		BuildTask(Segment* segment);
		~BuildTask();

		void ProviderRun(const BuildMessage& buildMessage);
		BuildTaskStatus ProviderGetResult(int timeout = INFINITE);

		bool HostGetBuildMessage(void** outMessage, u32_t* size);
		void HostSetBuildStatus(BuildTaskStatus buildStatus);

	private:

		void setStatus(BuildTaskStatus status);
		void setBuildMessage(const void* message, u32_t size);

		BuildTaskStatus getStatus();
		void getBuildMessage(void** outMessage, u32_t* size);

		void* getPtr(SegmentOffset offset);
		

	private:
		Segment* _segment;
	};

	extern "C"
	{
		CORE_API u32_t HostGetBuildTaskCount(const wchar_t* buildId);
		CORE_API bool HostGetBuildMessage(const wchar_t* buildId, int buildTaskId, void** outMessage, u32_t* size);
		CORE_API bool HostSetBuildTaskStatus(const wchar_t* buildId, int buildTaskId, u32_t status);
	} 
}