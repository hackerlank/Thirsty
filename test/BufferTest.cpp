#include "net/Buffer.h"
#include <gtest/gtest.h>
#include <string>
#include <iterator>
#include <algorithm>

using namespace std;

TEST(Buffer, BufferCtor)
{    
    string s = "hello, kitty";
    Buffer buf(s.data(), s.length());
    EXPECT_EQ(s.size(), buf.size());
    EXPECT_NE(s.data(), (const char*)buf.data());
    auto r = strncmp(s.c_str(), (const char*)buf.data(), s.size());
    EXPECT_TRUE(r == 0);

    Buffer b2(s.size());
    EXPECT_EQ(s.size(), buf.size());
    memcpy(b2.data(), s.data(), s.size());
    r = strncmp(s.c_str(), (const char*)b2.data(), s.size());
    EXPECT_TRUE(r == 0);
}

TEST(Buffer, Iterator)
{
    string s = "a quick fox jumps over the lazy log";
    Buffer buf(s.data(), s.size());

    string s2(buf.begin(), buf.end());
    EXPECT_EQ(s, s2);

    string s3;
    for_each(begin(buf), end(buf), [&](Buffer::value_type ch)
    {
        s3.push_back(ch);
    });
    EXPECT_EQ(s, s3);
}

