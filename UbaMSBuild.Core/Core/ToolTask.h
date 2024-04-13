#pragma once
#include "Types.h"
#include <nlohmann/json.hpp>

namespace uba_msbuild {

	class Segment;


	enum SegmentOffset : u32_t
	{
		SegmentOffset_BuildStatus = 0,
		SegmentOffset_ToolTaskSize = 1,
		SegmentOffset_ToolTask = 2,
	};

	enum ToolTaskStatus : u32_t
	{
		Created = 0,
		Cancelled = 1,
		RanToCompletion = 2,
		Faulted = 3,
		Running = 4,
	};

	class ToolTask
	{
	public:
		ToolTask(Segment* segment);
		~ToolTask();

		void Run(const nlohmann::json& json);
		ToolTaskStatus GetResult(int timeout = INFINITE);

		bool GetToolTask(void** outToolTask, u32_t* size);
		void SetToolTaskStatus(ToolTaskStatus buildStatus);

	private:

		void setStatus(ToolTaskStatus status);
		void setToolTask(const void* message, u32_t size);

		ToolTaskStatus getStatus();
		void getToolTask(void** outToolTask, u32_t* size);

		void* getPtr(SegmentOffset offset);
		

	private:
		Segment* _segment;
	};
}