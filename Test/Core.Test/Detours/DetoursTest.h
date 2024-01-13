#pragma once
#include <gtest/gtest.h>
#include "../../../Shared/Core/Exports/Detours.h"

class DetoursTest : public ::testing::Test
{
protected:
    DetoursTest() {}
    virtual ~DetoursTest() {}

    virtual void SetUp() override {}
    virtual void TearDown() override {}
};


TEST(DetoursTest, CreateProcessWithDllsTest)
{
    wchar_t commandLine[] = L"C:\\Windows\\System32\\notepad.exe";
    ubavs::CreateProcessWithDll(commandLine, GetEnvironmentStringsW(), NULL, L"D:\\Git\\UBAVS\\x64\\Debug\\Core.dll");

    EXPECT_EQ(1, 1);
}
