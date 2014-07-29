/*
 * Copyright 2014 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/Conv.h"
#include "core/Foreach.h"
#include "Benchmark.h"
#include <climits>
#include <limits>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

using namespace std;


TEST(Conv, Integral2Integral)
{
    // Same size, different signs
    int64_t s64 = numeric_limits<uint8_t>::max();
    EXPECT_EQ(to<uint8_t>(s64), s64);

    s64 = numeric_limits<int8_t>::max();
    EXPECT_EQ(to<int8_t>(s64), s64);

    s64 = numeric_limits<int32_t>::max();
    EXPECT_ANY_THROW(to<int8_t>(s64));    // convert large to less
}

TEST(Conv, Floating2Floating)
{
    float f1 = 1e3;
    double d1 = to<double>(f1);
    EXPECT_EQ(f1, d1);

    double d2 = 23.0;
    auto f2 = to<float>(d2);
    EXPECT_EQ(double(f2), d2);

    double invalidFloat = std::numeric_limits<double>::max();
    EXPECT_ANY_THROW(to<float>(invalidFloat));
    invalidFloat = -std::numeric_limits<double>::max();
    EXPECT_ANY_THROW(to<float>(invalidFloat));

    try
    {
        auto shouldWork = to<float>(std::numeric_limits<double>::min());
        // The value of `shouldWork' is an implementation defined choice
        // between the following two alternatives.
        EXPECT_TRUE(shouldWork == std::numeric_limits<float>::min() ||
            shouldWork == 0.f);
    }
    catch (...)
    {
        EXPECT_TRUE(false);
    }
}


TEST(Conv, testIntegral2String)
{
    char ch = 'X';
    EXPECT_EQ(to<string>(ch), "X");
    EXPECT_EQ(to<string>(uint8_t(-0)), "0");
    EXPECT_EQ(to<string>(uint8_t(UCHAR_MAX)), "255");

    EXPECT_EQ(to<string>(int16_t(-0)), "0");
    EXPECT_EQ(to<string>(int16_t(SHRT_MIN)), "-32768");
    EXPECT_EQ(to<string>(int16_t(SHRT_MAX)), "32767");
    EXPECT_EQ(to<string>(uint16_t(0)), "0");
    EXPECT_EQ(to<string>(uint16_t(USHRT_MAX)), "65535");

    EXPECT_EQ(to<string>(int32_t(-0)), "0");
    EXPECT_EQ(to<string>(int32_t(INT_MIN)), "-2147483648");
    EXPECT_EQ(to<string>(int32_t(INT_MAX)), "2147483647");
    EXPECT_EQ(to<string>(uint32_t(0)), "0");
    EXPECT_EQ(to<string>(uint32_t(UINT_MAX)), "4294967295");

    EXPECT_EQ(to<string>(int64_t(-0)), "0");
    EXPECT_EQ(to<string>(int64_t(INT64_MIN)), "-9223372036854775808");
    EXPECT_EQ(to<string>(int64_t(INT64_MAX)), "9223372036854775807");
    EXPECT_EQ(to<string>(uint64_t(0)), "0");
    EXPECT_EQ(to<string>(uint64_t(UINT64_MAX)), "18446744073709551615");

    enum A{ x = 0, y = 420, z = 2147483647 };
    EXPECT_EQ(to<string>(x), "0");
    EXPECT_EQ(to<string>(y), "420");
    EXPECT_EQ(to<string>(z), "2147483647");
}

TEST(Conv, testFloating2String)
{
    EXPECT_EQ(to<string>(0.0f), "0");
    EXPECT_EQ(to<string>(0.5), "0.5");
    EXPECT_EQ(to<string>(10.25f), "10.25");
    EXPECT_EQ(to<string>(1.123e10), "11230000000");
}

TEST(Conv, testString2String)
{
    string s = "hello, kitty";
    EXPECT_EQ(to<string>(s), s);
    StringPiece piece(s.data(), 5);
    EXPECT_EQ(to<string>(piece), "hello");
}


TEST(Conv, testString2Integral)
{
    EXPECT_EQ(to<uint8_t>(string("255")), 255);
    EXPECT_EQ(to<int16_t>(string("-32768")), -32768);
    EXPECT_EQ(to<uint16_t>(string("65535")), 65535);
    EXPECT_EQ(to<int32_t>(string("-2147483648")), -2147483648L);
    EXPECT_ANY_THROW(to<int16_t>(string("-2147483648")));
    EXPECT_EQ(to<uint32_t>(string("4294967295")), 4294967295);
    EXPECT_EQ(to<int64_t>(string("-9223372036854775808")), -9223372036854775808LL);
    EXPECT_EQ(to<uint64_t>(string("18446744073709551615")), 18446744073709551615ULL);
    EXPECT_EQ(to<uint64_t>(string("18446744073709551615")), 18446744073709551615ULL);

    // empty string
    string s = "";
    StringPiece pc(s);
    EXPECT_ANY_THROW(to<int>(pc));

    // corrupted string
    s = "-1";
    StringPiece pc2(s.data(), s.data() + 1); // Only  "-"
    EXPECT_ANY_THROW(to<int64_t>(&pc2));
}

TEST(Conv, testString2Bool)
{
    EXPECT_FALSE(to<bool>(string("0")));
    EXPECT_FALSE(to<bool>(string("  000  ")));

    EXPECT_FALSE(to<bool>(string("n")));
    EXPECT_FALSE(to<bool>(string("no")));
    EXPECT_FALSE(to<bool>(string("false")));
    EXPECT_FALSE(to<bool>(string("False")));
    EXPECT_FALSE(to<bool>(string("  fAlSe")));
    EXPECT_FALSE(to<bool>(string("F")));
    EXPECT_FALSE(to<bool>(string("off")));

    EXPECT_TRUE(to<bool>(string("1")));
    EXPECT_TRUE(to<bool>(string("  001 ")));
    EXPECT_TRUE(to<bool>(string("y")));
    EXPECT_TRUE(to<bool>(string("yes")));
    EXPECT_TRUE(to<bool>(string("\nyEs\t")));
    EXPECT_TRUE(to<bool>(string("true")));
    EXPECT_TRUE(to<bool>(string("True")));
    EXPECT_TRUE(to<bool>(string("T")));
    EXPECT_TRUE(to<bool>(string("on")));

    EXPECT_ANY_THROW(to<bool>(string("")));
    EXPECT_ANY_THROW(to<bool>(string("2")));
    EXPECT_ANY_THROW(to<bool>(string("11")));
    EXPECT_ANY_THROW(to<bool>(string("19")));
    EXPECT_ANY_THROW(to<bool>(string("o")));
    EXPECT_ANY_THROW(to<bool>(string("fal")));
    EXPECT_ANY_THROW(to<bool>(string("tru")));
    EXPECT_ANY_THROW(to<bool>(string("ye")));
    EXPECT_ANY_THROW(to<bool>(string("yes foo")));
    EXPECT_ANY_THROW(to<bool>(string("bar no")));
    EXPECT_ANY_THROW(to<bool>(string("one")));
    EXPECT_ANY_THROW(to<bool>(string("true_")));
    EXPECT_ANY_THROW(to<bool>(string("bogus_token_that_is_too_long")));

    // Test with strings that are not NUL terminated.
    const char buf[] = "01234";
    EXPECT_FALSE(to<bool>(StringPiece(buf, buf + 1)));  // "0"
    EXPECT_TRUE(to<bool>(StringPiece(buf + 1, buf + 2)));  // "1"
    const char buf2[] = "one two three";
    EXPECT_TRUE(to<bool>(StringPiece(buf2, buf2 + 2)));  // "on"
    const char buf3[] = "false";
    EXPECT_ANY_THROW(to<bool>(StringPiece(buf3, buf3 + 3))); // "fal"

    // Test the StringPiece* API
    const char buf4[] = "001foo";
    StringPiece sp4(buf4);
    EXPECT_TRUE(to<bool>(&sp4));
    EXPECT_EQ(buf4 + 3, sp4.begin());
    const char buf5[] = "0012";
    StringPiece sp5(buf5);
    EXPECT_ANY_THROW(to<bool>(&sp5));
    EXPECT_EQ(buf5, sp5.begin());
}

TEST(Conv, StringPieceToDouble)
{
    string s = "2134123.125 zorro";
    StringPiece pc(s);
    EXPECT_EQ(to<double>(&pc), 2134123.125);
    EXPECT_EQ(pc, " zorro");

    EXPECT_ANY_THROW(to<double>(StringPiece(s)));
    EXPECT_EQ(to<double>(StringPiece(s.data(), pc.data())), 2134123.125);

    // Test NaN conversion
    try
    {
        to<double>("not a number");
        EXPECT_TRUE(false);
    }
    catch (const std::exception &)
    {
    }

    EXPECT_TRUE(std::isnan(to<double>("NaN")));
    EXPECT_EQ(to<double>("inf"), numeric_limits<double>::infinity());
    EXPECT_EQ(to<double>("infinity"), numeric_limits<double>::infinity());
    EXPECT_ANY_THROW(to<double>("infinitX"));
    EXPECT_EQ(to<double>("-inf"), -numeric_limits<double>::infinity());
    EXPECT_EQ(to<double>("-infinity"), -numeric_limits<double>::infinity());
    EXPECT_ANY_THROW(to<double>("-infinitX"));

    try
    {
        to<double>(""); // empty string to double
        EXPECT_TRUE(false);
    }
    catch (const std::exception &)
    {
    }
}

TEST(Conv, testVariadicTo)
{
    string s;
    toAppend(&s);
    toAppend(&s, "Lorem ipsum ", 1234, string(" dolor amet "), 567.89, '!');
    EXPECT_EQ(s, "Lorem ipsum 1234 dolor amet 567.89!");

    s = to<string>("");
    EXPECT_TRUE(s.empty());

    s = to<string>("Lorem ipsum ", nullptr, 1234, " dolor amet ", 567.89, '.');
    EXPECT_EQ(s, "Lorem ipsum 1234 dolor amet 567.89.");
}

TEST(Conv, testVariadicToDelim)
{
    string s("Yukkuri shiteitte ne!!!");

    string charDelim = toDelim<string>('$', s);
    EXPECT_EQ(charDelim, s);

    string strDelim = toDelim<string>(string(">_<"), s);
    EXPECT_EQ(strDelim, s);

    s.clear();
    toAppendDelim(&s,
        ":", "Lorem ipsum ", 1234, string(" dolor amet "), 567.89, '!');
    EXPECT_EQ(s, "Lorem ipsum :1234: dolor amet :567.89:!");

    s = toDelim<string>(':', "");
    EXPECT_TRUE(s.empty());

    s = toDelim<string>(
        ":", "Lorem ipsum ", nullptr, 1234, " dolor amet ", 567.89, '.');
    EXPECT_EQ(s, "Lorem ipsum ::1234: dolor amet :567.89:.");
}

TEST(Conv, testEnum)
{
    enum A { x = 42, y = 420, z = 65 };
    auto i = to<int>(x);
    EXPECT_EQ(i, 42);
    auto j = to<char>(x);
    EXPECT_EQ(j, 42);
    try
    {
        auto i = to<char>(y);
        LOG(ERROR) << static_cast<unsigned int>(i);
        EXPECT_TRUE(false);
    }
    catch (std::exception& e)
    {
        //LOG(INFO) << e.what();
    }

    EXPECT_EQ("42", to<string>(x));
    EXPECT_EQ("420", to<string>(y));
    EXPECT_EQ("65", to<string>(z));

    auto i2 = to<A>(42);
    EXPECT_EQ(i, x);
    auto j2 = to<A>(100);
    EXPECT_EQ(j2, 100);
    try 
    {
        auto i = to<A>(5000000000L);
        EXPECT_TRUE(false);
    }
    catch (std::exception& e)
    {
        //LOG(INFO) << e.what();
    }

    enum E : uint32_t { m = 3000000000U };
    auto u = to<uint32_t>(m);
    EXPECT_EQ(u, 3000000000U);
    auto s = to<string>(m);
    EXPECT_EQ("3000000000", s);
    auto e = to<E>(3000000000U);
    EXPECT_EQ(e, m);
    try
    {
        auto i = to<int32_t>(m);
        LOG(ERROR) << to<uint32_t>(m);
        EXPECT_TRUE(false);
    }
    catch (std::exception& e)
    {
    }
}

TEST(Conv, NewUint64ToString)
{
    char buf[21];

#define THE_GREAT_EXPECTATIONS(n, len)                      \
    do {                                                    \
        EXPECT_EQ((len), uint64ToBufferUnsafe((n), buf));   \
        buf[(len)] = 0;                                     \
        auto s = string(#n);                                \
        s = s.substr(0, s.size() - 2);                      \
        EXPECT_EQ(s, buf);                                  \
        } while (0)

    THE_GREAT_EXPECTATIONS(0UL, 1);
    THE_GREAT_EXPECTATIONS(1UL, 1);
    THE_GREAT_EXPECTATIONS(12UL, 2);
    THE_GREAT_EXPECTATIONS(123UL, 3);
    THE_GREAT_EXPECTATIONS(1234UL, 4);
    THE_GREAT_EXPECTATIONS(12345UL, 5);
    THE_GREAT_EXPECTATIONS(123456UL, 6);
    THE_GREAT_EXPECTATIONS(1234567UL, 7);
    THE_GREAT_EXPECTATIONS(12345678UL, 8);
    THE_GREAT_EXPECTATIONS(123456789UL, 9);
    THE_GREAT_EXPECTATIONS(1234567890UL, 10);
    THE_GREAT_EXPECTATIONS(12345678901UL, 11);
    THE_GREAT_EXPECTATIONS(123456789012UL, 12);
    THE_GREAT_EXPECTATIONS(1234567890123UL, 13);
    THE_GREAT_EXPECTATIONS(12345678901234UL, 14);
    THE_GREAT_EXPECTATIONS(123456789012345UL, 15);
    THE_GREAT_EXPECTATIONS(1234567890123456UL, 16);
    THE_GREAT_EXPECTATIONS(12345678901234567UL, 17);
    THE_GREAT_EXPECTATIONS(123456789012345678UL, 18);
    THE_GREAT_EXPECTATIONS(1234567890123456789UL, 19);
    THE_GREAT_EXPECTATIONS(18446744073709551614UL, 20);
    THE_GREAT_EXPECTATIONS(18446744073709551615UL, 20);

#undef THE_GREAT_EXPECTATIONS
}
