// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Core/Core.h"
#include "Core/SharedMemory.h"
#include "Messages/BuildMessage.pb.h"

#include <detours/detours.h>
#include <boost/algorithm/string.hpp>
#include <boost/locale/encoding_utf.hpp>

std::wstring gChannelName = L"UBAVS";

BOOL (*True_CreateProcessW)(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
) = ::CreateProcessW;

BOOL WINAPI Detoured_CreateProcessW(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
    DEBUG_LOG(L"Detoured ---> %s\n", lpCommandLine);
    if (boost::icontains(lpCommandLine, "msbuild.exe"))
    {
        DEBUG_LOG(L"Should be detour. ---> %s\n", lpCommandLine);

        return DetourCreateProcessWithDll(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, "D:\\Git\\UBAVS\\Binaries\\x64\\Debug\\Core.Test\\Core.dll", True_CreateProcessW);
    }
    else if (boost::icontains(lpCommandLine, "tracker.exe"))
    {
        DEBUG_LOG(L"Should be send to host. ---> %s\n", lpCommandLine);

        ubavs::BuildMessage message;
        DEBUG_LOG(L"CommandLine : %s\n", lpCommandLine);
        std::string commandLine = boost::locale::conv::utf_to_utf<char>(lpCommandLine);
        message.set_commandline(commandLine);

        DEBUG_LOG(L"CurrentDirectory : %s\n", lpCurrentDirectory);
        std::string currentDirectory = boost::locale::conv::utf_to_utf<char>(lpCurrentDirectory);
        message.set_currentdirectory(boost::locale::conv::utf_to_utf<char>(currentDirectory));

        const wchar_t* lpEnv = static_cast<const wchar_t*>(lpEnvironment);
        while (*lpEnv != L'\0')
        {
            DEBUG_LOG(L"Env : %s\n", lpEnv);
            std::string env = boost::locale::conv::utf_to_utf<char>(lpEnv);
            message.add_environment(env);

            lpEnv += env.size() + 1;
        }

        try
        {
            std::string serializedMessage = message.SerializeAsString();
            std::wstring serializedMessageW = boost::locale::conv::utf_to_utf<wchar_t>(serializedMessage);

            DEBUG_LOG(L"Send ---> %s\n", serializedMessageW);
        }
        catch (const std::exception& e)
        {
			DEBUG_LOG(L"Exception : %s\n", e.what());
		}
        catch (...)
        {
            DEBUG_LOG(L"Exception : %s\n");
        }
    }

    return True_CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (DetourIsHelperProcess())
        return TRUE;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DEBUG_LOG(L"DLL_PROCESS_ATTACH\n");
        DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)True_CreateProcessW, Detoured_CreateProcessW);
		DetourTransactionCommit();
		break;
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)True_CreateProcessW, Detoured_CreateProcessW);
        DetourTransactionCommit();
        break;
    }
    return TRUE;
}

