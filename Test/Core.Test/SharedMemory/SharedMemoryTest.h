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
        ubavs::CloseMessageChannel(channelName);
    }

    virtual void TearDown() override
    {
		ubavs::CloseMessageChannel(channelName);
	}

public:
    const wchar_t* channelName = L"TestChannel";
};


TEST_F(ShardMemoryTest, WriteReadTest)
{
    bool timeout = ubavs::WriteMessage(channelName, L"hello world.");
    EXPECT_EQ(timeout, false);

    wchar_t* message = nullptr;
    timeout = ubavs::ReadMessage(channelName, &message);
    EXPECT_EQ(timeout, false);

    EXPECT_TRUE(wcscmp(message, L"hello world.") == 0);

    ubavs::FreeMessage(message);
}

TEST_F(ShardMemoryTest, TimeoutTest)
{
    bool timeout = ubavs::WriteMessage(channelName, L"hello world.");
    EXPECT_EQ(timeout, false);

    timeout = ubavs::WriteMessage(channelName, L"hello world.", 1000);
    EXPECT_EQ(timeout, true);

    wchar_t* message = nullptr;
    timeout = ubavs::ReadMessage(channelName, &message, 1000);
    EXPECT_EQ(timeout, false);

    timeout = ubavs::ReadMessage(channelName, &message, 1000);
    EXPECT_EQ(timeout, true);
}
