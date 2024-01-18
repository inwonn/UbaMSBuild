#pragma once
#include "../Core/Core.h"

namespace ubavs {

	class Segment;

	enum MemoryMappedFileAccessState : unsigned int
	{
		None = 0,
		HostRead = 1,
		HostWrite = 2,
		HostDone = 3,
		ProviderRead = 4,
		ProviderWrite = 5,
		ProviderDone = 6,
	};

	enum BuildTaskResult : unsigned int
	{
		None1 = 0,
		Success = 1,
		Fail = 2,
		Cancel = 3
	};

	MemoryMappedFileAccessState GetState(Segment* segment);
	void SetState(Segment* segment, MemoryMappedFileAccessState state);
	unsigned int GetSize(Segment* segment);
	void Read(Segment* segment, void* data, int size);
	void Write(Segment* segment, void* data, int size);

	extern "C"
	{
		CORE_API unsigned int HostGetBuildTaskCount(const wchar_t* communicationChannelname);
		CORE_API bool HostGetBuildTask(const wchar_t* communicationChannelname, int idx, void* data);
		CORE_API bool HostSetBuildTaskResult(const wchar_t* communicationChannelname, int idx, int result);
	} 
}