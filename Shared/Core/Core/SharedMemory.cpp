#include "pch.h"
#include "Debug.h"
#include "SharedMemory.h"
#include <Windows.h>
#include <format>

namespace ubavs {

	ScopedLock::ScopedLock(HANDLE hMutex)
	{
		_hMutex = hMutex;
		WaitForSingleObject(_hMutex, INFINITE);
	}

	ScopedLock::~ScopedLock()
	{
		ReleaseMutex(_hMutex);
		CloseHandle(_hMutex);
	}

	void Page::Write(const void* buffer, int size)
	{
		header.size = size;
		memcpy(data, buffer, size);
	}

	void Page::Read(void** buffer, int* size)
	{
		*size = header.size;
		memcpy(data, *buffer, header.size);
	}

	SharedMemory::SharedMemory(const wchar_t* name)
	{
		_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
		if (_hMapFile == NULL)
		{
			LARGE_INTEGER capacity;
			capacity.QuadPart = PAGE_SIZE * PAGE_COUNT;
			_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_RESERVE, capacity.HighPart, capacity.LowPart, name);
			if (_hMapFile == NULL)
			{
				// LOG
			}

			_header = (SharedMemoryHeader*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, PAGE_SIZE);
			if (_header == nullptr)
			{
				// LOG
			}

			if (!::VirtualAlloc(_header, PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE))
			{
				// LOG
			}
		}
		else
		{
			_header = (SharedMemoryHeader*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, PAGE_SIZE);
		}
	}

	SharedMemory::~SharedMemory()
	{
		CloseHandle(_hMapFile);
	}

	Page* SharedMemory::Alloc()
	{
		LARGE_INTEGER offset;
		offset.QuadPart = PAGE_SIZE;

		for (int idx = 0; idx < _header->capacity; idx++)
		{
			Page* page = (Page*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, PAGE_SIZE);
			if (page != nullptr && page->header.state == PageState::Free)
			{
				return page;
			}
			offset.QuadPart += PAGE_SIZE;
		}

		Page* page = (Page*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, PAGE_SIZE);
		if (page != nullptr)
		{
			if (!::VirtualAlloc(page, PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE))
			{
				// LOG
			}

			page->header.state = PageState::Allocated;
			page->header.size = 0;
			page->header.index = _header->capacity;
			_header->capacity++;

			return page;
		}
		

		return nullptr;
	}
	void SharedMemory::Free(Page* page)
	{
		page->header.state = PageState::Free;
		page->header.size = 0;
		page->header.index = 0;
	}

	void SharedMemory::ForEach(std::function<void(Page*)> callback)
	{
		LARGE_INTEGER offset;
		offset.QuadPart = PAGE_SIZE;

		for (int idx = 0; idx < _header->capacity; idx++)
		{
			Page* page = (Page*)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, offset.HighPart, offset.LowPart, PAGE_SIZE);
			if (page != nullptr)
			{
				callback(page);
			}
			offset.QuadPart += PAGE_SIZE;
		}
	}

	void Test()
	{
		SharedMemory shm1(L"UBAVS");
		SharedMemory shm2(L"UBAVS");
		SharedMemory shm3(L"UBAVS");

		Page* p2 = shm2.Alloc();
		Page* p3 = shm3.Alloc();

		const char* msg = "Hello World";
		p2->Write(msg, strlen(msg));

		const char* msg2 = "Hello World2";
		p3->Write(msg2, strlen(msg2));

		shm1.ForEach([&](Page* page) {
			if (page->header.state == PageState::Allocated)
			{
				std::string str((char*)page->data, page->header.size);
				shm1.Free(page);
			}
		});
	}
}
