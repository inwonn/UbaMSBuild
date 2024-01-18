#include "pch.h"
#include "Communication.h"
#include <map>
#include <string>
#include "../Core/MemoryMappedFile.h"

namespace ubavs
{
	MemoryMappedFileAccessState GetState(Segment* segment)
	{
		return (MemoryMappedFileAccessState)InterlockedCompareExchange((uint32_t*)segment->data, MemoryMappedFileAccessState::None, MemoryMappedFileAccessState::None);
	}

	void SetState(Segment* segment, MemoryMappedFileAccessState state)
	{
		InterlockedExchange((uint32_t*)segment->data, state);
	}

	unsigned int GetSize(Segment* segment)
	{
		uint8_t* data = segment->data + sizeof(uint32_t);
		return (unsigned int)*data;
	}

	void Read(Segment* segment, void* data, int size)
	{
		memcpy_s(data, size, segment->data + sizeof(uint32_t) * 2, size);
	}

	void Write(Segment* segment, void* data, int size)
	{
		memcpy_s(segment->data + sizeof(uint32_t), sizeof(uint32_t), &size, sizeof(uint32_t));
		memcpy_s(segment->data + sizeof(uint32_t) * 2, size, data, size);
	}

	MemoryMappedFile mappedFile(L"UBAVS");
	unsigned int HostGetBuildTaskCount(const wchar_t* communicationChannelname)
	{
		//MemoryMappedFile mappedFile(communicationChannelname);

		return mappedFile.GetCapacity();
	}

	bool HostGetBuildTask(const wchar_t* communicationChannelname, int idx, void* data)
	{
		//MemoryMappedFile mappedFile(communicationChannelname);

		Segment* segment = mappedFile.Get(idx);
		if (segment != nullptr && segment->IsValid())
		{
			if (GetState(segment) == MemoryMappedFileAccessState::ProviderDone)
			{
				SetState(segment, MemoryMappedFileAccessState::HostRead);
				int size = GetSize(segment);
				Read(segment, data, size);
				return true;
			}
		}
		return false;
	}

	CORE_API bool HostSetBuildTaskResult(const wchar_t* communicationChannelname, int idx, int result)
	{
		//MemoryMappedFile mappedFile(communicationChannelname);

		Segment* segment = mappedFile.Get(idx);
		if (segment != nullptr && segment->IsValid())
		{
			SetState(segment, MemoryMappedFileAccessState::HostWrite);
			Write(segment, &result, sizeof(uint32_t));
			SetState(segment, MemoryMappedFileAccessState::HostDone);
			return true;
		}
		return false;
	}
}