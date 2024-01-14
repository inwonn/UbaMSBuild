#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/algorithm/string.hpp>

namespace ubavs {
	class SharedMemory
	{
		typedef boost::interprocess::allocator<wchar_t, boost::interprocess::managed_shared_memory::segment_manager> char_allocator_type;
		typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, char_allocator_type> shared_memory_string_type;

	public:
		static void Clear(const wchar_t* segmentName);

		explicit SharedMemory(const wchar_t* segmentName, bool isHost = false);
		~SharedMemory();

		void Read(std::wstring* out);
		void Write(const std::wstring& in);

	private:
		bool canRead();
		bool canWrite();

	private:
		bool _isHost;
		std::wstring _name;
		boost::interprocess::managed_shared_memory _segment;
		boost::interprocess::interprocess_mutex* _mutex;
		boost::interprocess::interprocess_condition* _readCV;
		boost::interprocess::interprocess_condition* _writeCV;
		shared_memory_string_type* _data;
	};
}