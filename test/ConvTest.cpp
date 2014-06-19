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
#include <climits>
#include <limits>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

using namespace std;


TEST(Conv, Integral2Integral) {
  // Same size, different signs
  int64_t s64 = numeric_limits<uint8_t>::max();
  EXPECT_EQ(to<uint8_t>(s64), s64);

  s64 = numeric_limits<int8_t>::max();
  EXPECT_EQ(to<int8_t>(s64), s64);

  s64 = numeric_limits<int32_t>::max();
  EXPECT_ANY_THROW(to<int8_t>(s64));    // convert large to less
}

TEST(Conv, Floating2Floating) {
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

  try {
    auto shouldWork = to<float>(std::numeric_limits<double>::min());
    // The value of `shouldWork' is an implementation defined choice
    // between the following two alternatives.
    EXPECT_TRUE(shouldWork == std::numeric_limits<float>::min() ||
                shouldWork == 0.f);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}


TEST(Conv, testIntegral2String) {
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

TEST(Conv, testFloating2String) {
    EXPECT_EQ(to<string>(0.0f), "0");
    EXPECT_EQ(to<string>(0.5), "0.5");
    EXPECT_EQ(to<string>(10.25f), "10.25");
    EXPECT_EQ(to<string>(1.123e10), "11230000000");
}

TEST(Conv, testString2String) {
    string s = "hello, kitty";
    EXPECT_EQ(to<string>(s), s);
    StringPiece piece(s.data(), 5);
    EXPECT_EQ(to<string>(piece), "hello");
}


TEST(Conv, testString2Integral)
{
    string s = "255";
    EXPECT_EQ(to<uint8_t>(s), 255);

    s = "-32768";
    EXPECT_EQ(to<int16_t>(s), -32768);
    s = "65535";
    EXPECT_EQ(to<uint16_t>(s), 65535);

    s = "-2147483648"; 
    EXPECT_EQ(to<int32_t>(s), -2147483648);
    EXPECT_ANY_THROW(to<int16_t>(s));
    s = "4294967295";
    EXPECT_EQ(to<uint32_t>(s), 4294967295);

    s = "-9223372036854775808";
    EXPECT_EQ(to<int64_t>(s), -9223372036854775808);
    s = "18446744073709551615";
    EXPECT_EQ(to<uint64_t>(s), 18446744073709551615);

    StringPiece p = s;
    EXPECT_EQ(to<uint64_t>(s), 18446744073709551615);
}
