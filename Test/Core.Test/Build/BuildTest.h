#pragma once
#include "../../../Shared/Core/Exports/Detours.h"

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
    }

public:
    std::filesystem::path _detoursLib;
    std::filesystem::path _solutionPath;
};


TEST_F(BuildTest, RebuildSolutionTest)
{
    SetEnvironmentVariableW(L"MSBUILDDISABLENODEREUSE", L"1");

    std::wstring commandline = std::format(L"\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\devenv.exe\" {} /Rebuild \"Debug|x64\"", _solutionPath.wstring().c_str());
    HANDLE hProcess = ubavs::CreateProcessWithDll((LPWSTR)commandline.c_str(), GetEnvironmentStringsW(), NULL, _detoursLib.wstring().c_str());
    EXPECT_NE(hProcess, INVALID_HANDLE_VALUE);

    EXPECT_EQ(WaitForSingleObject(hProcess, INFINITE), 0);

    DWORD exieCode;
    EXPECT_TRUE(GetExitCodeProcess(hProcess, &exieCode));

    EXPECT_EQ(exieCode, 0);

    std::filesystem::path outputBinaryPath = _solutionPath.parent_path() / "x64" / "Debug" / "BuildTest.exe";
    std::filesystem::path outputPdbPath = _solutionPath.parent_path() / "x64" / "Debug" / "BuildTest.pdb";

    EXPECT_TRUE(std::filesystem::exists(outputBinaryPath));
    EXPECT_TRUE(std::filesystem::exists(outputPdbPath));
}