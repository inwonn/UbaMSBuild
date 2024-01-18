#include "pch.h"
#include "Debug.h"
#include "MemoryMappedFile.h"
#include <Windows.h>
#include <format>

namespace ubavs {

	bool Segment::IsValid()
	{
		return InterlockedCompareExchange((uint32_t*)& header.state, 0, 0) == SegmentState::Allocated;
	}

	void Segment::Commit()
	{
		InterlockedCompareExchange((uint32_t*)&header.state, SegmentState::Free, SegmentState::Allocated);
	}

	void Segment::Release()
	{
		InterlockedCompareExchange((uint32_t*)&header.state, SegmentState::Allocated, SegmentState::Free);
	}

	MemoryMappedFile::MemoryMappedFile(const wchar_t* name)
	{
		_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
		if (_hMapFile == NULL)
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

			if (!::VirtualAlloc(_header, SEGMENT_SIZE, MEM_COMMIT, PAGE_READWRITE))
			{
				// LOG
			}
		}
		else
		{
			_header = (MemoryMappedFileHeader*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SEGMENT_SIZE);
		}
	}

	MemoryMappedFile::~MemoryMappedFile()
	{
		CloseHandle(_hMapFile);
	}

	Segment* MemoryMappedFile::Commit()
	{
		uint32_t capacity = InterlockedCompareExchange(&_header->capacity, 0, 0);
		LARGE_INTEGER offset;

		for (uint32_t idx = 0; idx < capacity; idx++)
		{
			offset.QuadPart = SEGMENT_SIZE * (idx + 1);
			Segment* segment = (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
			if (segment != nullptr && segment->IsValid())
			{
				return segment;
			}
		}

		capacity = InterlockedIncrement(&_header->capacity);
		offset.QuadPart = SEGMENT_SIZE * (capacity);
		Segment* segment = (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
		if (segment == nullptr)
		{
			// LOG
		}

		if (!::VirtualAlloc(segment, SEGMENT_SIZE, MEM_COMMIT, PAGE_READWRITE))
		{
			// LOG
		}
		
		segment->Commit();

		return segment;
	}

	void MemoryMappedFile::Release(Segment* segment)
	{
		segment->Release();
	}

	uint32_t MemoryMappedFile::GetCapacity()
	{
		return InterlockedCompareExchange((uint32_t*)& _header->capacity, 0, 0);
	}

	Segment* MemoryMappedFile::Get(uint32_t idx)
	{
		LARGE_INTEGER offset;
		offset.QuadPart = SEGMENT_SIZE * (idx + 1);
		return (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
	}

	void Test()
	{
		MemoryMappedFile shm1(L"UBAVS");
		MemoryMappedFile shm2(L"UBAVS");
		MemoryMappedFile shm3(L"UBAVS");

		Segment* p2 = shm2.Commit();
		Segment* p3 = shm3.Commit();

		const char* msg = "Hello World";
		//p2->Write(msg, strlen(msg));

		const char* msg2 = "Hello World2";
		//p3->Write(msg2, strlen(msg2));
	}
}
