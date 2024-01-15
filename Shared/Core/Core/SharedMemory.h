#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/algorithm/string.hpp>

namespace ubavs {

	class SharedMemory
	{
		typedef boost::interprocess::allocator<wchar_t, boost::interprocess::managed_shared_memory::segment_manager> char_allocator_t;
		typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, char_allocator_t> shared_memory_string_t;

	public:
		enum class ErrorType
		{
			NoError = 0,
			Timeout = 1
		};

		static SharedMemory& Get();
		static void Create();
		static void Release();
		
		ErrorType Read(std::wstring* out, int timeoutMilliseconds = -1);
		ErrorType Write(const std::wstring& in, int timeoutMilliseconds = -1);

	private:
		explicit SharedMemory();

		bool canRead() const;
		bool canWrite() const;

		bool isInitialized() const { return _data != nullptr; }

		void init();
		void release();

	private:
		static const int SHARED_MEMORY_SIZE = 65536;
		static SharedMemory gSharedMemory;

		bool _isInitialized;
		boost::interprocess::managed_shared_memory _segment;
		boost::interprocess::interprocess_mutex* _mutex;
		boost::interprocess::interprocess_condition* _readCondition;
		boost::interprocess::interprocess_condition* _writeCondition;
		bool* _hasAnyData;
		shared_memory_string_t* _data;
	};
}