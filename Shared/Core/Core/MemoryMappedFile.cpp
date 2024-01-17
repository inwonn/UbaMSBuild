#include "pch.h"
#include "Debug.h"
#include "MemoryMappedFile.h"
#include <Windows.h>
#include <format>

namespace ubavs {

	void Segment::Write(const void* buffer, int size)
	{
		header.size = size;
		memcpy(data, buffer, size);
	}

	void Segment::Read(void** buffer, int* size)
	{
		*size = header.size;
		memcpy(data, *buffer, header.size);
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

	Segment* MemoryMappedFile::Alloc()
	{
		LARGE_INTEGER offset;
		offset.QuadPart = SEGMENT_SIZE;

		for (int idx = 0; idx < _header->capacity; idx++)
		{
			Segment* segment = (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
			if (segment != nullptr && segment->header.state == SegmentState::Free)
			{
				return segment;
			}
			offset.QuadPart += SEGMENT_SIZE;
		}

		Segment* segment = (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
		if (segment != nullptr)
		{
			if (!::VirtualAlloc(segment, SEGMENT_SIZE, MEM_COMMIT, PAGE_READWRITE))
			{
				// LOG
			}

			segment->header.state = SegmentState::Allocated;
			segment->header.size = 0;
			segment->header.index = _header->capacity;
			_header->capacity++;

			return segment;
		}
		

		return nullptr;
	}
	void MemoryMappedFile::Free(Segment* segment)
	{
		segment->header.state = SegmentState::Free;
		segment->header.size = 0;
		segment->header.index = 0;
	}

	void MemoryMappedFile::ForEach(std::function<void(Segment*)> callback)
	{
		LARGE_INTEGER offset;
		offset.QuadPart = SEGMENT_SIZE;

		for (int idx = 0; idx < _header->capacity; idx++)
		{
			Segment* segment = (Segment*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, SEGMENT_SIZE);
			if (segment != nullptr)
			{
				callback(segment);
			}
			offset.QuadPart += SEGMENT_SIZE;
		}
	}

	void Test()
	{
		MemoryMappedFile shm1(L"UBAVS");
		MemoryMappedFile shm2(L"UBAVS");
		MemoryMappedFile shm3(L"UBAVS");

		Segment* p2 = shm2.Alloc();
		Segment* p3 = shm3.Alloc();

		const char* msg = "Hello World";
		p2->Write(msg, strlen(msg));

		const char* msg2 = "Hello World2";
		p3->Write(msg2, strlen(msg2));

		shm1.ForEach([&](Segment* segment) {
			if (segment->header.state == SegmentState::Allocated)
			{
				std::string str((char*)segment->data, segment->header.size);
				shm1.Free(segment);
			}
		});
	}
}
