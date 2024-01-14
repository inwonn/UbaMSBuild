#include "pch.h"
#include "Messages.h"
#include "../Core/SharedMemory.h"

#include <boost/locale/encoding_utf.hpp>

namespace ubavs {
	void CloseMessageChannel(const wchar_t* channelName)
	{
		SharedMemory sharedMemory(channelName, true);
	}

	const wchar_t* ReadMessage(const wchar_t* channelName)
	{
		SharedMemory sharedMemory(channelName, true);
		std::wstring loadedMessage;
		sharedMemory.Read(&loadedMessage);

		size_t messageLength = loadedMessage.length();
		wchar_t* message = new wchar_t[messageLength + 1];
		wcscpy_s(message, messageLength + 1, loadedMessage.c_str());

		return message;
	}

	void FreeMessage(wchar_t* message)
	{
		if (message != nullptr)
		{
			delete[] message;
			message = nullptr;
		}
	}

	void WriteMessage(const wchar_t* channelName, const wchar_t* message)
	{
		SharedMemory sharedMemory(channelName);

		sharedMemory.Write(message);
	}
}
