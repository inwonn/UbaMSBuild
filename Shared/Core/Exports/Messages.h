#pragma once
#include "../Core/Core.h"
#include <string>

namespace ubavs {

	extern "C" {
		CORE_API void CloseMessageChannel(const wchar_t* channelName);
		CORE_API const wchar_t* ReadMessage(const wchar_t* channelName);
		CORE_API void FreeMessage(wchar_t* message);
		CORE_API void WriteMessage(const wchar_t* channelName, const wchar_t* message);
	}
}

