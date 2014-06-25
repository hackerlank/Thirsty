#include "core/Random.h"
#include <gtest/gtest.h>

TEST(Random, Uniform)
{
    Random64 rnd;
    for (int i = 0; i < 100; i++)
    {
        EXPECT_LT(rnd.Uniform(100), 100);
    }
}

TEST(Random, Next)
{
    Random64 rnd;
    for (int i = 0; i < 100; i++)
    {
        auto n = rnd.Next();
    }
}

TEST(Random, OneIn)
{
    Random64 rnd;
    for (int i = 0; i < 100; i++)
    {
        auto n = rnd.OneIn(i);
    }
}
