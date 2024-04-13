#pragma once
#include <Core/Detours.h>
#include <Core/Export.h>

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
        _detoursLib = currentModulePath.parent_path() / "UbaMSBuild.Core.dll";
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
    DWORD pid = -1;
    HANDLE hProcess = uba_msbuild::CreateProcessWithDll((LPWSTR)commandline.c_str(), buildId, _detoursLib.wstring().c_str(), &pid);
    EXPECT_NE(hProcess, INVALID_HANDLE_VALUE);

    DWORD exieCode;
    int receivedCount = 0;
    std::vector<uba_msbuild::u8_t> data(65536, 0);
    unsigned int dataSize = 0;
    do {
        unsigned int taskCount = uba_msbuild::GetToolTaskCount(buildId);
        for (unsigned int taskId = 0; taskId < taskCount; ++taskId)
        {
            void* dataPtr = data.data();
            if (uba_msbuild::GetToolTaskStatus(buildId, taskId) == 4/*Running*/, uba_msbuild::GetToolTask(buildId, taskId, &dataPtr, &dataSize))
			{
				++receivedCount;
                printf("received ---> %d\n", dataSize);
                uba_msbuild::SetToolTaskStatus(buildId, taskId, 2/*RanToCompletion*/);
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