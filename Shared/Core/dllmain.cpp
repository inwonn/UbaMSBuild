// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Core/BuildTask.h"
#include "Core/Debug.h"
#include "Core/Detours.h"
#include "Core/MemoryMappedFile.h"
#include "Generated/BuildMessage.pb.h"

#include <detours/detours.h>
#include <boost/algorithm/string.hpp>
#include <boost/locale/encoding_utf.hpp>

using namespace ubavs;

std::wstring g_buildId = L"UBAVS";
std::wstring g_detoursLib = L"D:\\Git\\UBAVS\\Binaries\\x64\\Debug\\Core.Test\\Core.dll";

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

        return CreateProcessWithDllEx(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, g_buildId.c_str(), g_detoursLib.c_str(), True_CreateProcessW);
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
            MemoryMappedFile mappedFile(g_buildId.c_str());
            ScopedSegment segment(mappedFile);
            if (segment.Get()->IsValid() == false)
            {
                DEBUG_LOG(L"segment is not valid\n");
                // LOG
            }

            BuildTask task(segment.Get());

            task.ProviderRun(message);
            BuildTaskStatus result = task.ProviderGetResult(5000);

            if (result == BuildTaskStatus_Completed)
			{
				DEBUG_LOG(L"Success\n");
			}
			else
			{
				DEBUG_LOG(L"Failed\n");
			}
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
    wchar_t path[MAX_PATH] = { 0 ,};
    if (GetModuleFileNameW(NULL, path, MAX_PATH))
    {
        if (boost::icontains(path, L"devenv.exe") == false &&
            boost::icontains(path, L"msbuild.exe") == false)
        {
            DEBUG_LOG(L"detour filtered %s\n", path);
            return TRUE;
        }
    }

    if (DetourIsHelperProcess())
        return TRUE;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)True_CreateProcessW, Detoured_CreateProcessW);
        DetourTransactionCommit();

        for (HMODULE hMod = NULL; (hMod = DetourEnumerateModules(hMod)) != NULL;) {
            ULONG cbData;
            PVOID pvData = DetourFindPayload(hMod, DetoursPayloadGuid, &cbData);

            if (pvData != NULL) {
                DetoursPayload* payload = (DetoursPayload*)(pvData);

                g_buildId = payload->buildId;
                g_detoursLib = payload->detoursLib;
                DEBUG_LOG(L"buildId : %s\n", payload->buildId);
                DEBUG_LOG(L"detoursLib : %s\n", payload->detoursLib);
                break;
            }
        }
        break;
    }	
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)True_CreateProcessW, Detoured_CreateProcessW);
        DetourTransactionCommit();
        break;
    }
    return TRUE;
}

