#pragma once
#include "../Core/Core.h"
#include <string>

namespace ubavs {

	extern "C" {
		CORE_API void CreateMessageChannel();
		CORE_API void ReleaseMessageChannel();
		CORE_API int ReadMessage(wchar_t** outMessage, int timeoutMilliseconds = -1);
		CORE_API int WriteMessage(const wchar_t* message, int timeoutMilliseconds = -1);
		CORE_API void FreeReadMessage(wchar_t* message);
	}
}

