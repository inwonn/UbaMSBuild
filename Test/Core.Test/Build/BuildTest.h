#pragma once
#include <Core/Detours.h>
#include <Core/BuildTask.h>
#include <Core/Types.h>

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
    const wchar_t* buildId = L"UbaBuildTestId";
    std::wstring commandline = std::format(L"\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\devenv.exe\" {} /Rebuild \"Debug|x64\"", _solutionPath.wstring().c_str());
    HANDLE hProcess = ubavs::CreateProcessWithDll((LPWSTR)commandline.c_str(), buildId, _detoursLib.wstring().c_str());
    EXPECT_NE(hProcess, INVALID_HANDLE_VALUE);

    DWORD exieCode;
    int receivedCount = 0;
    std::vector<ubavs::u8_t> data(65536, 0);
    unsigned int dataSize = 0;
    do {
        unsigned int taskCount = ubavs::HostGetBuildTaskCount(buildId);
        for (unsigned int taskId = 0; taskId < taskCount; ++taskId)
        {
            void* dataPtr = data.data();
            if (ubavs::HostGetBuildMessage(buildId, taskId, &dataPtr, &dataSize))
			{
				++receivedCount;
                printf("received ---> %d\n", dataSize);
                ubavs::HostSetBuildTaskStatus(buildId, taskId, 1);
			}
        }
        GetExitCodeProcess(hProcess, &exieCode);
    } while (exieCode == STILL_ACTIVE);

    EXPECT_EQ(receivedCount, 2);
    EXPECT_EQ(exieCode, 0);

    std::filesystem::path outputBinaryPath = _solutionPath.parent_path() / "x64" / "Debug" / "BuildTest.exe";
    std::filesystem::path outputPdbPath = _solutionPath.parent_path() / "x64" / "Debug" / "BuildTest.pdb";

    EXPECT_TRUE(std::filesystem::exists(outputBinaryPath));
    EXPECT_TRUE(std::filesystem::exists(outputPdbPath));
}