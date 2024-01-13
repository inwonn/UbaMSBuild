#include "pch.h"

#ifdef _DEBUG
#include "Debug.h"
#include <stdio.h>
#include <string>
#include <format>

namespace ubavs {

	void OutputDebugStringFormat(const wchar_t* format, const wchar_t* file, const wchar_t* function, int line, ...)
	{
		if (format)
		{
			va_list args;
			va_start(args, format);
			int len = _vscwprintf(format, args) + 1;
			std::wstring buffer(len, L'\0');
			vswprintf_s((wchar_t*)buffer.c_str(), len, format, args);
			va_end(args);


			buffer = std::format(L"[ubavs][{}] {} [{} (Line:{}))]", function, buffer, file, line);

			OutputDebugString(buffer.c_str());
		}
	}
}

#endif
