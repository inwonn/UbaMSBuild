#include "pch.h"
#include "Messages.h"
#include "../Core/SharedMemory.h"

#include <boost/locale/encoding_utf.hpp>

namespace ubavs {
	void CreateMessageChannel()
	{
		SharedMemory::Create();
	}
	void ReleaseMessageChannel()
	{
		SharedMemory::Release();
	}

	int ReadMessage(wchar_t** outMessage, int timeoutMilliseconds /*= -1*/)
	{
		std::wstring loadedMessage;
		SharedMemory::ErrorType error = SharedMemory::Get().Read(&loadedMessage, timeoutMilliseconds);

		if (error == SharedMemory::ErrorType::NoError)
		{
			size_t messageLength = loadedMessage.length();
			*outMessage = new wchar_t[messageLength + 1];
			wcscpy_s(*outMessage, messageLength + 1, loadedMessage.c_str());
		}
		
		return (int)error;
	}

	void FreeReadMessage(wchar_t* message)
	{
		if (message != nullptr)
		{
			delete[] message;
			message = nullptr;
		}
	}

	int WriteMessage(const wchar_t* message, int timeoutMilliseconds /*= -1*/)
	{
		return (int)SharedMemory::Get().Write(message, timeoutMilliseconds);
	}
}
