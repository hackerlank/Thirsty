// Copyright (C) 2014-2015 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include <string>
#include <assert.h>

enum CodePage
{
    PAGE_ACP = 0,
    PAGE_BIG5 = 950,
    PAGE_GB2312 = 936,
    PAGE_GB18030 = 54936,
    PAGE_UTF8 = 65001,
};

// Convert a multi-bytes string(e.g. GBK, BIG5, UTF-8) to a wide character string.
bool MultiByteToWide(const std::string& strMbs,
                     CodePage cp,
                     int flags,
                     std::wstring* strOut);

// Convert a wide character string to multi-bytes string(e.g. GBK, BIG5, UTF-8)
bool WideToMultibyte(const std::wstring& strWide,
                     CodePage cp,
                     int flags,
                     bool useDefault,
                     std::string* strOut);

// Convert a wide character string to a UTF-8 string
inline std::string WideToUtf8(const std::wstring& strWide)
{
    std::string strUtf8;
    if (!WideToMultibyte(strWide, PAGE_UTF8, 0, false, &strUtf8))
    {
        assert(!"WideToUtf8() failed");
    }
    return strUtf8;
}

// Convert a UTF-8 string to a wide character string
inline std::wstring Utf8ToWide(const std::string& strUtf8)
{
    std::wstring strWide;
    if (!MultiByteToWide(strUtf8, PAGE_UTF8, 0, &strWide))
    {
        assert(!"Utf8ToWide() failed");
    }
    return strWide;
}

// Convert a wide character string to a GBK string
inline std::string WideToGbk(const std::wstring& strWide)
{
    std::string strGbk;
    if (!WideToMultibyte(strWide, PAGE_GB2312, 0, false, &strGbk))
    {
        assert(!"WideToGbk() failed");
    }
    return strGbk;
}

// Convert a GBK string to a wide character string
inline std::wstring GbkToWide(const std::string& strGbk)
{
    std::wstring strWide;
    if (!MultiByteToWide(strGbk, PAGE_GB2312, 0, &strWide))
    {
        assert(!"GbkToWide() failed");
    }
    return strWide;
}

// Convert a GBK string to UTF-8 string
inline std::string GbkToUtf8(const std::string& strGbk)
{
    return WideToUtf8(GbkToWide(strGbk));
}

// Convert a UTF-8 string to a GBK string
inline std::string Utf8ToGbk(const std::string& strUtf8)
{
    return WideToGbk(Utf8ToWide(strUtf8));
}

// Convert a wide character string to a BIG5 string
inline std::string WideToBig5(const std::wstring& strWide)
{
    std::string strBig5;
    if (!WideToMultibyte(strWide, PAGE_BIG5, 0, false, &strBig5))
    {
        assert(!"WideToBig5() failed");
    }
    return strBig5;
}

// Convert a BIG5 string to a wide character string
inline std::wstring Big5ToWide(const std::string& strBig5)
{
    std::wstring strWide;
    if (!MultiByteToWide(strBig5, PAGE_BIG5, 0, &strWide))
    {
        assert(!"Big5ToWide() failed");
    }
    return strWide;
}

// Convert a Big5 string to UTF-8 string
inline std::string Big5ToUtf8(const std::string& strBig5)
{
    return WideToUtf8(Big5ToWide(strBig5));
}

// Convert a UTF-8 string to a Big5 string
inline std::string Utf8ToBig5(const std::string& strUtf8)
{
    return WideToBig5(Utf8ToWide(strUtf8));
}

// Maps one input character string to another with specified local
bool MapLocalString(const std::wstring& strSource, 
                    int dstLocal, 
                    std::wstring* strOut);

// Map Simplified-Chinese to Traditional-Chinese
// e.g  "陈旧"  ->   "陳舊"
std::wstring MapToTraditonal(const std::wstring& simplified);

// Input a GBK string and return a Big5 string
inline std::string MapToTraditonal(const std::string& simplified)
{
    return WideToBig5(MapToTraditonal(GbkToWide(simplified)));
}

// Map Traditional-Chinese to Simplified-Chinese
// e.g  "陳舊" ->   "陈旧" 
std::wstring MapToSimplified(const std::wstring& traditional);

// Input a Big5 string and return a GBK string
inline std::string MapToSimplified(const std::string& traditional)
{
    return WideToGbk(MapToSimplified(Big5ToWide(traditional)));
}
