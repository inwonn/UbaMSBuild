#pragma once
#include "../../../Shared/Core/Exports/Messages.h"

#include <gtest/gtest.h>
#include <filesystem>

class ShardMemoryTest : public ::testing::Test
{
protected:
    ShardMemoryTest() {}
    virtual ~ShardMemoryTest() {}

    virtual void SetUp() override
	{
        ubavs::ReleaseMessageChannel();
        ubavs::CreateMessageChannel();
	}

    virtual void TearDown() override
	{
        ubavs::ReleaseMessageChannel();
	}
};


TEST_F(ShardMemoryTest, WriteReadTest)
{
    int error = ubavs::WriteMessage(L"hello world.");
    EXPECT_EQ(error, 0/*noerror*/);

    wchar_t* message = nullptr;
    error = ubavs::ReadMessage(&message);
    EXPECT_EQ(error, 0/*noerror*/);

    EXPECT_TRUE(wcscmp(message, L"hello world.") == 0);

    ubavs::FreeReadMessage(message);
}

TEST_F(ShardMemoryTest, TimeoutTest)
{
    int error = ubavs::WriteMessage(L"hello world.", 1000);
    EXPECT_EQ(error, 0/*noerror*/);

    error = ubavs::WriteMessage(L"hello world.", 1000);
    EXPECT_EQ(error, 1/*timeout*/);

    wchar_t* message = nullptr;
    error = ubavs::ReadMessage(&message, 1000);
    EXPECT_EQ(error, 0/*noerror*/);

    error = ubavs::ReadMessage(&message, 1000);
    EXPECT_EQ(error, 1/*timeout*/);
}
