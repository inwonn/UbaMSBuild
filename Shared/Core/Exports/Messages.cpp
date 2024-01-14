#include "pch.h"
#include "Messages.h"
#include "../Core/SharedMemory.h"

#include <boost/locale/encoding_utf.hpp>

namespace ubavs {
	void CloseMessageChannel(const wchar_t* channelName)
	{
		SharedMemory sharedMemory(channelName, true);
	}

	bool ReadMessage(const wchar_t* channelName, wchar_t** outMessage, int timeoutMilliseconds /*= -1*/)
	{
		SharedMemory sharedMemory(channelName, true);
		std::wstring loadedMessage;
		bool timeout = sharedMemory.Read(&loadedMessage, timeoutMilliseconds);

		if (!timeout)
		{
			size_t messageLength = loadedMessage.length();
			*outMessage = new wchar_t[messageLength + 1];
			wcscpy_s(*outMessage, messageLength + 1, loadedMessage.c_str());
		}
		
		return timeout;
	}

	void FreeMessage(wchar_t* message)
	{
		if (message != nullptr)
		{
			delete[] message;
			message = nullptr;
		}
	}

	bool WriteMessage(const wchar_t* channelName, const wchar_t* message, int timeoutMilliseconds /*= -1*/)
	{
		SharedMemory sharedMemory(channelName);

		return sharedMemory.Write(message, timeoutMilliseconds);
	}
}
