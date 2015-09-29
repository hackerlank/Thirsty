// Copyright (C) 2014-2015 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include "Convert.h"
#include <Windows.h>


bool MultiByteToWide(const std::string& strMbs,
                     CodePage cp,
                     int flags,
                     std::wstring* strOut)
{
    assert(strOut != NULL);
    if (strMbs.empty())
    {
        return true;
    }
    int count = MultiByteToWideChar(cp, 0, strMbs.data(), strMbs.length(), NULL, 0);
    if (count > 0)
    {
        strOut->resize(count);
        count = MultiByteToWideChar(cp, flags, strMbs.data(), strMbs.length(),
            const_cast<wchar_t*>(strOut->data()), strOut->length());
    }
    return count > 0;
}


bool WideToMultibyte(const std::wstring& strWide,
                     CodePage cp,
                     int flags,
                     bool useDefault,
                     std::string* strOut)
{
    assert(strOut != NULL);
    if (strWide.empty())
    {
        return true;
    }
    BOOL bUseDefault = 0;
    int count = WideCharToMultiByte(cp, 0, strWide.data(), strWide.length(),
        NULL, 0, NULL, NULL);
    if (count > 0)
    {
        strOut->resize(count);
        BOOL* pUseDefault = (useDefault ? &bUseDefault : NULL);
        count = WideCharToMultiByte(cp, flags, strWide.data(), strWide.length(),
            const_cast<char*>(strOut->data()), strOut->length(), NULL, pUseDefault);
    }
    return (count > 0 && !bUseDefault);
}

bool MapLocalString(const std::wstring& strSource, int dstLocal, std::wstring* strOut)
{
    assert(strOut != NULL);
    if (strSource.empty())
    {
        return true;
    }
    int count = LCMapStringW(LOCALE_SYSTEM_DEFAULT, dstLocal, strSource.data(), 
        strSource.length(), NULL, 0);
    if (count > 0)
    {
        strOut->resize(count);
        count = LCMapStringW(LOCALE_SYSTEM_DEFAULT, dstLocal, strSource.data(),
            strSource.length(), const_cast<wchar_t*>(strOut->data()), strOut->length());
    }
    return count > 0;
}

std::wstring MapToTraditonal(const std::wstring& simplified)
{
    std::wstring traditional;
    if (!MapLocalString(simplified, LCMAP_TRADITIONAL_CHINESE, &traditional))
    {
        assert("MapToTraditonal() failed");
        traditional.clear();
    }
    return traditional;
}

std::wstring MapToSimplified(const std::wstring& traditional)
{
    std::wstring simplified;
    if (!MapLocalString(traditional, LCMAP_SIMPLIFIED_CHINESE, &simplified))
    {
        assert("MapToSimplified() failed");
        simplified.clear();
    }
    return simplified;
}
