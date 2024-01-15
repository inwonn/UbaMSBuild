#include "pch.h"
#include "Debug.h"
#include "SharedMemory.h"
#include <boost/bind/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread_time.hpp>

namespace ubavs {

	SharedMemory SharedMemory::gSharedMemory;
	SharedMemory& SharedMemory::Get()
	{
		return gSharedMemory;
	}

	void SharedMemory::Create()
	{
		if (SharedMemory::Get().isInitialized())
			std::runtime_error("Shared memory already initialized");

		SharedMemory::Get().init();
	}

	void SharedMemory::Release()
	{
		SharedMemory::Get().release();
	}

	SharedMemory::ErrorType SharedMemory::Read(std::wstring* out, int timeoutMilliseconds /*= -1*/)
	{
		SharedMemory::ErrorType error = SharedMemory::ErrorType::NoError;
		BOOST_TRY {
			boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*_mutex);
			if (timeoutMilliseconds != -1)
			{
				boost::system_time const timePoint = boost::get_system_time() + boost::posix_time::milliseconds(timeoutMilliseconds);
				if (!_readCondition->timed_wait(lock, timePoint, boost::bind(&SharedMemory::canRead, this)))
				{
					DEBUG_LOG(L"Read timed out\n");
					error = SharedMemory::ErrorType::Timeout;
				}
				else
				{
					out->assign(_data->begin(), _data->end());
					*_hasAnyData = false;
				}
			}
			else
			{
				_readCondition->wait(lock, boost::bind(&SharedMemory::canRead, this));
				out->assign(_data->begin(), _data->end());
				*_hasAnyData = false;
			}
		}
		BOOST_CATCH(std::exception& e)
		{
			DEBUG_LOG(L"Exception: %s\n", e.what());
		}
		BOOST_CATCH_END

		_writeCondition->notify_one();

		return error;
	}

	SharedMemory::ErrorType SharedMemory::Write(const std::wstring& in, int timeoutMilliseconds /*= -1*/)
	{
		SharedMemory::ErrorType error = SharedMemory::ErrorType::NoError;
		BOOST_TRY{
			boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*_mutex);
			if (timeoutMilliseconds != -1)
			{
				boost::system_time const timePoint = boost::get_system_time() + boost::posix_time::milliseconds(timeoutMilliseconds);
				if (!_writeCondition->timed_wait(lock, timePoint, boost::bind(&SharedMemory::canWrite, this)))
				{
					DEBUG_LOG(L"Write timed out\n");
					error = SharedMemory::ErrorType::Timeout;
				}
				else
				{
					_data->assign(in.begin(), in.end());
					*_hasAnyData = true;
				}
			}
			else
			{
				_writeCondition->wait(lock, boost::bind(&SharedMemory::canWrite, this));
				_data->assign(in.begin(), in.end());
				*_hasAnyData = true;
			}
		}
		BOOST_CATCH(std::exception& e)
		{
			DEBUG_LOG(L"Exception: %s\n", e.what());
		}
		BOOST_CATCH_END

		_readCondition->notify_one();

		return error;
	}

	SharedMemory::SharedMemory()
		: _isInitialized(false)
		, _data(nullptr)
	{
	}

	void SharedMemory::init()
	{
		_segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, L"UBAVS", SHARED_MEMORY_SIZE);
		_mutex = _segment.find_or_construct<boost::interprocess::interprocess_mutex>("lock")();
		_readCondition = _segment.find_or_construct<boost::interprocess::interprocess_condition>("read")();
		_writeCondition = _segment.find_or_construct<boost::interprocess::interprocess_condition>("write")();
		_hasAnyData = _segment.find_or_construct<bool>("any")();
		_data = _segment.find_or_construct<shared_memory_string_t>("data")(L"", _segment.get_segment_manager());
		_data->resize(10000);
	}

	void SharedMemory::release()
	{
		boost::interprocess::shared_memory_object::remove(L"UBAVS");
	}

	inline bool SharedMemory::canRead() const
	{
		return *_hasAnyData;
	}

	inline bool SharedMemory::canWrite() const
	{
		return !*_hasAnyData;
	}
}
