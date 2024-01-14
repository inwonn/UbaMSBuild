#include "pch.h"
#include "SharedMemory.h"
#include <boost/bind/bind.hpp>

namespace ubavs {

	void SharedMemory::Clear(const wchar_t* segmentName)
	{
		boost::interprocess::shared_memory_object::remove(segmentName);
	}

	SharedMemory::SharedMemory(const wchar_t* segmentName, bool isHost)
		: _isHost(isHost)
		, _name(segmentName)
	{
		_segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, segmentName, 65536);
		_mutex = _segment.find_or_construct<boost::interprocess::interprocess_mutex>("lock")();
		_readCV = _segment.find_or_construct<boost::interprocess::interprocess_condition>("read")();
		_writeCV = _segment.find_or_construct<boost::interprocess::interprocess_condition>("write")();
		_data = _segment.find_or_construct<shared_memory_string_type>("data")(L"", _segment.get_segment_manager());
	}

	SharedMemory::~SharedMemory()
	{
		if (_isHost) boost::interprocess::shared_memory_object::remove(_name.c_str());
	}

	void SharedMemory::Read(std::wstring* out)
	{
		if (!_isHost) throw std::exception("Client cannot read from shared memory");

		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*_mutex);
		_readCV->wait(lock, boost::bind(&SharedMemory::canRead, this));

		out->assign(_data->c_str(), _data->size());
		_data->clear();

		_writeCV->notify_one();
	}

	void SharedMemory::Write(const std::wstring& in)
	{
		if (_isHost) throw std::exception("Host cannot write to shared memory");

		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*_mutex);
		_writeCV->wait(lock, boost::bind(&SharedMemory::canWrite, this));

		_data->assign(in.c_str());

		_readCV->notify_one();
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
