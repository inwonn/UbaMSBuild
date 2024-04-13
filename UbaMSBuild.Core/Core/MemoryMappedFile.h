#pragma once
#include "Types.h"

namespace uba_msbuild {

	static const int SEGMENT_SIZE = 65536;
	static const int SEGMENT_COUNT = 32;

	enum SegmentState : u32_t
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
		u8_t data[SEGMENT_SIZE - sizeof(Header)];

	private:
		Header header;
	};

	class MemoryMappedFile
	{
	public:
		struct MemoryMappedFileHeader
		{
			u32_t allocCapacity = 0;
			u32_t capacity = 0;
		};

	public:
		MemoryMappedFile(const MemoryMappedFile&) = delete;
		MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		MemoryMappedFile(const wchar_t* name);
		~MemoryMappedFile();

		Segment* Commit();
		void Release(Segment* Segment);

		u32_t GetCapacity();
		Segment* Get(u32_t idx);

	private:
		u32_t GetAllocCapacity();

	private:
		HANDLE _hMapFile;
		MemoryMappedFileHeader* _header;
	};

	class ScopedSegment
	{
	public:
		ScopedSegment(MemoryMappedFile& mappedFile) 
			: _mappedFile(mappedFile) {
			_segment = mappedFile.Commit();
		}
		~ScopedSegment() {
			_mappedFile.Release(_segment);
		}

		Segment* Get() { return _segment; }

	private:
		MemoryMappedFile& _mappedFile;
		Segment* _segment;
	};
}