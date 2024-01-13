#include "pch.h"

#ifdef _DEBUG
#include "Debug.h"
#include <stdio.h>
#include <string>
#include <format>
#include <filesystem>

namespace ubavs {

	void OutputDebugStringFormat(const wchar_t* filename, int line, const wchar_t* format, ...)
	{
		if (format)
		{
			va_list args;
			va_start(args, format);
			int len = _vscwprintf(format, args) + 1;
			std::wstring buffer(len, L'\0');
			vswprintf_s((wchar_t*)buffer.c_str(), len, format, args);
			va_end(args);

			std::wstring output = std::format(L"[ubavs][{} Line:{}] {}", filename, line, buffer);

			OutputDebugString(output.c_str());
		}
	}
}

#endif
