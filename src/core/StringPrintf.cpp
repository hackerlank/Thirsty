// Copyright 2012 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//

#include "StringPrintf.h"
#include <cstdarg>
#include <algorithm>


inline void stringPrintfImpl(std::string& output, const char* format, va_list ap)
{
    // First try with a small fixed size buffer
    static const int kSpaceLength = 128;
    char space[kSpaceLength];

    // It's possible for methods that use a va_list to invalidate
    // the data in it upon use.  The fix is to make a copy
    // of the structure before using it and use that copy instead.
    va_list backup_ap;
    va_copy(backup_ap, ap);
    int result = vsnprintf(space, kSpaceLength, format, backup_ap);
    va_end(backup_ap);

    if (result < kSpaceLength)
    {
        if (result >= 0) {
            // Normal case -- everything fit.
            output.append(space, result);
            return;
        }

        // Error or MSVC running out of space.  MSVC 8.0 and higher
        // can be asked about space needed with the special idiom below:
        va_copy(backup_ap, ap);
        result = vsnprintf(NULL, 0, format, backup_ap);
        va_end(backup_ap);

        if (result < 0)
        {
            // Just an error.
            return;
        }
    }

    // Increase the buffer size to the size requested by vsnprintf,
    // plus one for the closing \0.
    int length = result + 1;
    char* buf = new char[length];

    // Restore the va_list before we use it again
    va_copy(backup_ap, ap);
    result = vsnprintf(buf, length, format, backup_ap);
    va_end(backup_ap);

    if (result >= 0 && result < length)
    {
        // It fit
        output.append(buf, result);
    }
    delete[] buf;
}


std::string stringPrintf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    std::string result;
    stringPrintfImpl(result, format, ap);
    va_end(ap);
    return std::move(result);
}


// Basic declarations; allow for parameters of strings and string
// pieces to be specified.
std::string& stringAppendf(std::string* output, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    stringPrintfImpl(*output, format, ap);
    va_end(ap);
    return *output;
}

void stringPrintf(std::string* output, const char* format, ...)
{
    output->clear();
    va_list ap;
    va_start(ap, format);
    stringPrintfImpl(*output, format, ap);
    va_end(ap);
};
