#include "pch.h"
#include "Debug.h"
#include "SharedMemory.h"
#include <boost/bind/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread_time.hpp>

namespace ubavs {

	void SharedMemory::Clear(const wchar_t* segmentName)
	{
		boost::interprocess::shared_memory_object::remove(segmentName);
	}

	SharedMemory::SharedMemory(const wchar_t* segmentName)
		: _name(segmentName)
		, _segment(boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, segmentName, 65536))
	{	
		_mutex = _segment.find_or_construct<boost::interprocess::interprocess_mutex>("lock")();
		_readCV = _segment.find_or_construct<boost::interprocess::interprocess_condition>("read")();
		_writeCV = _segment.find_or_construct<boost::interprocess::interprocess_condition>("write")();
		_data = _segment.find_or_construct<shared_memory_string_type>("data")(L"", _segment.get_segment_manager());
	}

	bool SharedMemory::Read(std::wstring* out, int timeoutMilliseconds /*= -1*/)
	{
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*_mutex);

		bool timeout = false;
		if (timeoutMilliseconds != -1)
		{
			boost::system_time const timePoint = boost::get_system_time() + boost::posix_time::milliseconds(timeoutMilliseconds);
			if (!_readCV->timed_wait(lock, timePoint, boost::bind(&SharedMemory::canRead, this)))
			{
				timeout = true;
				DEBUG_LOG(L"Read timed out\n");
				
			}
			else
			{
				out->assign(_data->c_str(), _data->size());
				_data->clear();
			}
		}
		else
		{
			_readCV->wait(lock, boost::bind(&SharedMemory::canRead, this));
			out->assign(_data->c_str(), _data->size());
			_data->clear();
		}

		_writeCV->notify_one();

		return timeout;
	}

	bool SharedMemory::Write(const std::wstring& in, int timeoutMilliseconds /*= -1*/)
	{
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*_mutex);

		bool timeout = false;
		if (timeoutMilliseconds != -1)
		{
			boost::system_time const timePoint = boost::get_system_time() + boost::posix_time::milliseconds(timeoutMilliseconds);
			if (!_writeCV->timed_wait(lock, timePoint, boost::bind(&SharedMemory::canWrite, this)))
			{
				timeout = true;
				DEBUG_LOG(L"Write timed out\n");
			}
			else
			{
				_data->assign(in.c_str());
			}
		}
		else
		{	
			_writeCV->wait(lock, boost::bind(&SharedMemory::canWrite, this));
			_data->assign(in.c_str());
		}

		_readCV->notify_one();
		return timeout;
	}

	inline bool SharedMemory::canRead()
	{
		return _data->size() > 0;
	}

	inline bool SharedMemory::canWrite()
	{
		return _data->size() == 0;
	}
}
