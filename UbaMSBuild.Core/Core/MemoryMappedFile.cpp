#include "pch.h"
#include "Debug.h"
#include "MemoryMappedFile.h"
#include "Types.h"

namespace uba_msbuild {

	bool Segment::IsValid()
	{
		SegmentState currentState = (SegmentState)InterlockedCompareExchange((u32_t*)&header.state, 0, 0);
		return currentState == SegmentState::Allocated;
	}

	void Segment::Commit()
	{
		InterlockedCompareExchange((u32_t*)&header.state, SegmentState::Allocated, SegmentState::Free);
	}

	void Segment::Release()
	{
		InterlockedCompareExchange((u32_t*)&header.state, SegmentState::Free, SegmentState::Allocated);
	}

	MemoryMappedFile::MemoryMappedFile(const wchar_t* name)
	{
		LARGE_INTEGER capacity;
		capacity.QuadPart = SEGMENT_SIZE * SEGMENT_COUNT;
		_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_RESERVE, capacity.HighPart, capacity.LowPart, name);
		if (_hMapFile == NULL)
		{
			// LOG
		}

		_header = (MemoryMappedFileHeader*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SEGMENT_SIZE);
		if (_header == nullptr)
		{
			// LOG
		}

		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			if (!::VirtualAlloc(_header, SEGMENT_SIZE, MEM_COMMIT, PAGE_READWRITE))
			{
				// LOG
			}
		};
	}

	MemoryMappedFile::~MemoryMappedFile()
	{
		CloseHandle(_hMapFile);
	}

	Segment* MemoryMappedFile::Commit()
	{
		u32_t capacity = GetCapacity();
		LARGE_INTEGER offset;

		for (u32_t idx = 0; idx < capacity; idx++)
		{
			offset.QuadPart = SEGMENT_SIZE * (idx + 1);
			Segment* segment = (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
			if (segment != nullptr && !segment->IsValid())
			{
				segment->Commit();
				return segment;
			}
		}

		u32_t allocCapacity = InterlockedIncrement(&_header->allocCapacity);
		offset.QuadPart = SEGMENT_SIZE * (allocCapacity);
		Segment* segment = (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
		if (segment == nullptr)
		{
			DEBUG_LOG(L"MapViewOfFile failed GLE(%ld)\n", GetLastError());
		}

		if (!::VirtualAlloc(segment, SEGMENT_SIZE, MEM_COMMIT, PAGE_READWRITE))
		{
			DEBUG_LOG(L"VirtualAlloc failedGLE(%ld)\n", GetLastError());
		}
		
		segment->Commit();
		DEBUG_LOG(L"new segment is allocated %d\n", allocCapacity);

		if (allocCapacity > GetCapacity())
		{
			InterlockedExchange(&_header->capacity, allocCapacity);
		}
		return segment;
	}

	void MemoryMappedFile::Release(Segment* segment)
	{
		segment->Release();
		DEBUG_LOG(L"segment is free\n");
	}

	u32_t MemoryMappedFile::GetCapacity()
	{
		return InterlockedCompareExchange((u32_t*)& _header->capacity, 0, 0);
	}

	Segment* MemoryMappedFile::Get(u32_t idx)
	{
		LARGE_INTEGER offset;
		offset.QuadPart = SEGMENT_SIZE * (idx + 1);
		return (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
	}

	u32_t MemoryMappedFile::GetAllocCapacity()
	{
		return InterlockedCompareExchange((u32_t*)&_header->allocCapacity, 0, 0);
	}
}
