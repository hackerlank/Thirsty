// Copyright (C) 2014-2015 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include <gtest/gtest.h>
#include "win32/Convert.h"

using namespace std;

const std::string globalGBKText =
"ѩ������һ�ִ���è�ƶ��ԭ���������в�ɽ����"
"ѩ�������Ĵ���ʳ�⶯��������о��м�Ϊ�����λ�ã��������ھ����ܾ��ڸ�ɽѩ���ϵĶ��"
"Ҳ��ʳ�⶯����Ϣ�غ��θ߶���ߵ�һ�֣�ѩ��������ں����ɽ������������������"
"���������׳��ë���Ũ�ܡ���С��Բ����Щ�����������ڼ�����������ɢ����"
"ѩ���ڴ���è�ƶ����������е����ͣ�ƽ��������27��54����峤75��130���ף�"
"β��ͨ�������峤�����ؿɴ�75���ͼΪ��ѩ����";

const std::wstring globalWideText =
L"ѩ������һ�ִ���è�ƶ��ԭ���������в�ɽ����"
L"ѩ�������Ĵ���ʳ�⶯��������о��м�Ϊ�����λ�ã��������ھ����ܾ��ڸ�ɽѩ���ϵĶ��"
L"Ҳ��ʳ�⶯����Ϣ�غ��θ߶���ߵ�һ�֣�ѩ��������ں����ɽ������������������"
L"���������׳��ë���Ũ�ܡ���С��Բ����Щ�����������ڼ�����������ɢ����"
L"ѩ���ڴ���è�ƶ����������е����ͣ�ƽ��������27��54����峤75��130���ף�"
L"β��ͨ�������峤�����ؿɴ�75���ͼΪ��ѩ����";


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
    const wstring Simplified = L"һ̨�¾ɵķ�����";
    const wstring Traditional = L"һ�_��f�ķ�����";
    EXPECT_EQ(MapToSimplified(""), "");
    EXPECT_EQ(MapToTraditonal(""), "");
    EXPECT_EQ(MapToSimplified(Traditional), Simplified);
    EXPECT_EQ(MapToTraditonal(Simplified), Traditional);
}