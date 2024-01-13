#pragma once
#include "../../../Shared/Core/Exports/Detours.h"

#include <gtest/gtest.h>
#include <filesystem>

class DetoursTest : public ::testing::Test
{
protected:
    DetoursTest() {}
    virtual ~DetoursTest() {}

    virtual void SetUp() override
    {
        TCHAR buffer[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, buffer, MAX_PATH);
        std::filesystem::path currentModulePath(buffer);
        
        _detoursLibPath = currentModulePath.parent_path() / "Core.dll";
    }

public:
    std::filesystem::path _detoursLibPath;
};


TEST_F(DetoursTest, CreateProcessWithDllsTest)
{
    wchar_t commandLine[] = L"C:\\Windows\\System32\\notepad.exe";
    HANDLE hProcess = ubavs::CreateProcessWithDll(commandLine, GetEnvironmentStringsW(), NULL, _detoursLibPath.wstring().c_str());
    EXPECT_NE(hProcess, INVALID_HANDLE_VALUE);

    Sleep(1000);

    EXPECT_TRUE(TerminateProcess(hProcess, 0));

    EXPECT_EQ(WaitForSingleObject(hProcess, INFINITE), 0);

    DWORD exieCode;
    EXPECT_TRUE(GetExitCodeProcess(hProcess, &exieCode));

    EXPECT_EQ(exieCode, 0);
}
