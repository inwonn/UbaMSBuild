#pragma once

#include "Core.h"
#include <Windows.h>
#include <functional>

namespace ubavs {

	static const int PAGE_SIZE = 65536;
	static const int PAGE_COUNT = 32;
	static const wchar_t* PAGE_PREFIX = L"GLOBAL\\UBAVS_PAGE_{}";
	static const wchar_t* SHARED_MEMORY_PREFIX = L"GLOBAL\\UBAVS_SHARED_MEMORY_{}";

	enum class PageState
	{
		Free = 0,
		Allocated = 1
	};

	class ScopedLock
	{
	public:
		ScopedLock(HANDLE hMutex);
		~ScopedLock();

	private:
		HANDLE _hMutex;
	};

	struct Page
	{
		struct Header
		{
			PageState state;
			int index;
			int size;
		};

		void Write(const void* buffer, int size);
		void Read(void** buffer, int* size);

		Header header;
		unsigned char data[PAGE_SIZE - sizeof(Header)];
	};

	class SharedMemory
	{
	public:
		struct SharedMemoryHeader
		{
			int capacity = 0;
		};

	public:
		SharedMemory(const SharedMemory&) = delete;
		SharedMemory& operator=(const SharedMemory&) = delete;
		SharedMemory(const wchar_t* name);
		~SharedMemory();

		Page* Alloc();
		void Free(Page* page);
		void ForEach(std::function<void(Page*)> callback);

	private:
		HANDLE _hMapFile;
		SharedMemoryHeader* _header;
	};

	extern "C" CORE_API void Test();
}