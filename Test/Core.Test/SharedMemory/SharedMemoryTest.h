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
    ubavs::WriteMessage(channelName, L"hello world.");

    const wchar_t* message = ubavs::ReadMessage(channelName);

    EXPECT_TRUE(wcscmp(message, L"hello world.") == 0);

    ubavs::FreeMessage((wchar_t*)message);
}
