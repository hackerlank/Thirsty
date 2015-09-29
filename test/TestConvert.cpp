// Copyright (C) 2014-2015 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include <gtest/gtest.h>
#include "win32/Convert.h"

using namespace std;

const std::string globalGBKText =
"雪豹，是一种大型猫科动物，原产于亚洲中部山区。"
"雪豹在珍贵的大形食肉动物的行列中具有极为特殊的位置，它是属于经常栖居于高山雪地上的动物，"
"也是食肉动物栖息地海拔高度最高的一种，雪豹有许多在寒冷的山区生长的生物特征，"
"他们身体粗壮、毛厚而浓密、耳小而圆，这些特征都有助于减少身体热量散发。"
"雪豹在大型猫科动物中属于中等体型，平均体重在27到54公斤，体长75到130厘米，"
"尾巴通常比身体长，体重可达75公斤。图为雄雪豹。";

const std::wstring globalWideText =
L"雪豹，是一种大型猫科动物，原产于亚洲中部山区。"
L"雪豹在珍贵的大形食肉动物的行列中具有极为特殊的位置，它是属于经常栖居于高山雪地上的动物，"
L"也是食肉动物栖息地海拔高度最高的一种，雪豹有许多在寒冷的山区生长的生物特征，"
L"他们身体粗壮、毛厚而浓密、耳小而圆，这些特征都有助于减少身体热量散发。"
L"雪豹在大型猫科动物中属于中等体型，平均体重在27到54公斤，体长75到130厘米，"
L"尾巴通常比身体长，体重可达75公斤。图为雄雪豹。";


TEST(Convert, WideToUtf8)
{
    EXPECT_EQ(WideToUtf8(L""), "");
    EXPECT_EQ(Utf8ToWide(""), L"");
    string strUtf8 = WideToUtf8(globalWideText);
    EXPECT_EQ(Utf8ToWide(strUtf8), globalWideText);
}

TEST(Convert, WideToGbk)
{
    EXPECT_EQ(WideToGbk(L""), "");
    EXPECT_EQ(GbkToWide(""), L"");
    EXPECT_EQ(WideToGbk(globalWideText), globalGBKText);
    EXPECT_EQ(GbkToWide(globalGBKText), globalWideText);
}

TEST(Convert, MapString)
{
    const wstring Simplified = L"一台陈旧的服务器";
    const wstring Traditional = L"一臺陳舊的服務器";
    EXPECT_EQ(MapToSimplified(""), "");
    EXPECT_EQ(MapToTraditonal(""), "");
    EXPECT_EQ(MapToSimplified(Traditional), Simplified);
    EXPECT_EQ(MapToTraditonal(Simplified), Traditional);
}