#pragma once

#include "Core.h"
#include <Windows.h>
#include <functional>

namespace ubavs {

	static const int SEGMENT_SIZE = 65536;
	static const int SEGMENT_COUNT = 32;

	enum SegmentState : uint32_t
	{
		Free = 0,
		Allocated = 1,
	};

	class Segment
	{
		friend class MemoryMappedFile;
	public:
		struct Header
		{
			SegmentState state;
		};

		bool IsValid();

	private:
		void Commit();
		void Release();

	public:
		uint8_t data[SEGMENT_SIZE - sizeof(Header)];

	private:
		Header header;
	};

	class MemoryMappedFile
	{
	public:
		struct MemoryMappedFileHeader
		{
			uint32_t capacity = 0;
		};

	public:
		MemoryMappedFile(const MemoryMappedFile&) = delete;
		MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		MemoryMappedFile(const wchar_t* name);
		~MemoryMappedFile();

		Segment* Commit();
		void Release(Segment* Segment);

		uint32_t GetCapacity();
		Segment* Get(uint32_t idx);

	private:
		HANDLE _hMapFile;
		MemoryMappedFileHeader* _header;
	};

	extern "C" CORE_API void Test();
}