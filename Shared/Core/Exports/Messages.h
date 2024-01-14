#pragma once
#include "../Core/Core.h"
#include <string>

namespace ubavs {

	extern "C" {
		CORE_API void CloseMessageChannel(const wchar_t* channelName);
		CORE_API bool ReadMessage(const wchar_t* channelName, wchar_t** outMessage, int timeoutMilliseconds = -1);
		CORE_API void FreeMessage(wchar_t* message);
		CORE_API bool WriteMessage(const wchar_t* channelName, const wchar_t* message, int timeoutMilliseconds = -1);
	}
}

