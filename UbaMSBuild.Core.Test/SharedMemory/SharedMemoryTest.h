#pragma once
#include "Core/MemoryMappedFile.h"

#include <gtest/gtest.h>
#include <filesystem>

class ShardMemoryTest : public ::testing::Test
{
protected:
    ShardMemoryTest() {}
    virtual ~ShardMemoryTest() {}

    virtual void SetUp() override
	{
	}

    virtual void TearDown() override
	{
	}
};


TEST_F(ShardMemoryTest, WriteReadTest)
{
}
