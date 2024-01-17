#pragma once

#include "Core.h"
#include <Windows.h>
#include <functional>

namespace ubavs {

	static const int SEGMENT_SIZE = 65536;
	static const int SEGMENT_COUNT = 32;

	enum class SegmentState
	{
		Free = 0,
		Allocated = 1
	};

	struct Segment
	{
		struct Header
		{
			SegmentState state;
			int index;
			int size;
		};

		void Write(const void* buffer, int size);
		void Read(void** buffer, int* size);

		Header header;
		unsigned char data[SEGMENT_SIZE - sizeof(Header)];
	};

	class MemoryMappedFile
	{
	public:
		struct MemoryMappedFileHeader
		{
			int capacity = 0;
		};

	public:
		MemoryMappedFile(const MemoryMappedFile&) = delete;
		MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		MemoryMappedFile(const wchar_t* name);
		~MemoryMappedFile();

		Segment* Alloc();
		void Free(Segment* Segment);
		void ForEach(std::function<void(Segment*)> callback);

	private:
		HANDLE _hMapFile;
		MemoryMappedFileHeader* _header;
	};

	extern "C" CORE_API void Test();
}