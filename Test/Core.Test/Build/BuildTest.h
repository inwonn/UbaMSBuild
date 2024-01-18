#pragma once
#include "../../../Shared/Core/Exports/Detours.h"
#include "../../../Shared/Core/Exports/Communication.h"

#include <gtest/gtest.h>
#include <filesystem>
#include <format>

class BuildTest : public ::testing::Test
{
protected:
    BuildTest() {}
    virtual ~BuildTest() {}

    virtual void SetUp() override
    {
        TCHAR buffer[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, buffer, MAX_PATH);
        std::filesystem::path currentModulePath(buffer);
        _detoursLib = currentModulePath.parent_path() / "Core.dll";
        _solutionPath = currentModulePath.parent_path() / "TestData" / "BuildTest" / "BuildTest.sln";

        SetEnvironmentVariableW(L"MSBUILDDISABLENODEREUSE", L"1");
    }

    virtual void TearDown() override
    {
    }

public:
    std::filesystem::path _detoursLib;
    std::filesystem::path _solutionPath;
};


TEST_F(BuildTest, RebuildSolutionTest)
{
    std::wstring commandline = std::format(L"\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\devenv.exe\" {} /Rebuild \"Debug|x64\"", _solutionPath.wstring().c_str());
    HANDLE hProcess = ubavs::CreateProcessWithDll((LPWSTR)commandline.c_str(), GetEnvironmentStringsW(), NULL, _detoursLib.wstring().c_str());
    EXPECT_NE(hProcess, INVALID_HANDLE_VALUE);

    DWORD exieCode;
    int receivedCount = 0;
    wchar_t message[65536] = { 0, };
    do {
        unsigned int size = ubavs::HostGetBuildTaskCount(L"UBAVS");
        for (unsigned int i = 0; i < size; ++i)
        {
            if (ubavs::HostGetBuildTask(L"UBAVS", i, message))
			{
                Sleep(1000);
                ubavs::HostSetBuildTaskResult(L"UBAVS", i, 1);
				++receivedCount;
			}
        }
        GetExitCodeProcess(hProcess, &exieCode);
    } while (exieCode == STILL_ACTIVE);

    /*EXPECT_EQ(receivedCount, 2);
    EXPECT_EQ(exieCode, 0);

    std::filesystem::path outputBinaryPath = _solutionPath.parent_path() / "x64" / "Debug" / "BuildTest.exe";
    std::filesystem::path outputPdbPath = _solutionPath.parent_path() / "x64" / "Debug" / "BuildTest.pdb";

    EXPECT_TRUE(std::filesystem::exists(outputBinaryPath));
    EXPECT_TRUE(std::filesystem::exists(outputPdbPath));*/
}