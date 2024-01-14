#pragma once
#include "../Core/Core.h"
#include <Windows.h>

namespace ubavs {

	extern "C" {
		CORE_API HANDLE CreateProcessWithDll(
			LPWSTR lpCommandLine,
			LPVOID lpEnvironment,
			LPCWSTR lpCurrentDirectory,
			LPCWSTR lpDllName);
	}
}

