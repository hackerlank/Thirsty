// Copyright 2012 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//

#include "Strings.h"
#include <cstdarg>
#include <cctype>
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
        if (result >= 0) 
        {
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


namespace {

struct PrettySuffix 
{
    const char* suffix;
    double val;
};

const PrettySuffix kPrettyTimeSuffixes[] = 
{
    { "s ", 1e0L },
    { "ms", 1e-3L },
    { "us", 1e-6L },
    { "ns", 1e-9L },
    { "ps", 1e-12L },
    { "s ", 0 },
    { 0, 0 },
};

const PrettySuffix kPrettyBytesMetricSuffixes[] = 
{
    { "TB", 1e12L },
    { "GB", 1e9L },
    { "MB", 1e6L },
    { "kB", 1e3L },
    { "B ", 0L },
    { 0, 0 },
};

const PrettySuffix kPrettyBytesBinarySuffixes[] = 
{
    { "TB", int64_t(1) << 40 },
    { "GB", int64_t(1) << 30 },
    { "MB", int64_t(1) << 20 },
    { "kB", int64_t(1) << 10 },
    { "B ", 0L },
    { 0, 0 },
};

const PrettySuffix kPrettyBytesBinaryIECSuffixes[] = 
{
    { "TiB", int64_t(1) << 40 },
    { "GiB", int64_t(1) << 30 },
    { "MiB", int64_t(1) << 20 },
    { "KiB", int64_t(1) << 10 },
    { "B  ", 0L },
    { 0, 0 },
};

const PrettySuffix kPrettyUnitsMetricSuffixes[] = 
{
    { "tril", 1e12L },
    { "bil", 1e9L },
    { "M", 1e6L },
    { "k", 1e3L },
    { " ", 0 },
    { 0, 0 },
};

const PrettySuffix kPrettyUnitsBinarySuffixes[] = 
{
    { "T", int64_t(1) << 40 },
    { "G", int64_t(1) << 30 },
    { "M", int64_t(1) << 20 },
    { "k", int64_t(1) << 10 },
    { " ", 0 },
    { 0, 0 },
};

const PrettySuffix kPrettyUnitsBinaryIECSuffixes[] = 
{
    { "Ti", int64_t(1) << 40 },
    { "Gi", int64_t(1) << 30 },
    { "Mi", int64_t(1) << 20 },
    { "Ki", int64_t(1) << 10 },
    { "  ", 0 },
    { 0, 0 },
};

const PrettySuffix kPrettySISuffixes[] = 
{
    { "Y", 1e24L },
    { "Z", 1e21L },
    { "E", 1e18L },
    { "P", 1e15L },
    { "T", 1e12L },
    { "G", 1e9L },
    { "M", 1e6L },
    { "k", 1e3L },
    { "h", 1e2L },
    { "da", 1e1L },
    { "d", 1e-1L },
    { "c", 1e-2L },
    { "m", 1e-3L },
    { "u", 1e-6L },
    { "n", 1e-9L },
    { "p", 1e-12L },
    { "f", 1e-15L },
    { "a", 1e-18L },
    { "z", 1e-21L },
    { "y", 1e-24L },
    { " ", 0 },
    { 0, 0 }
};

const PrettySuffix* const kPrettySuffixes[PRETTY_NUM_TYPES] = 
{
    kPrettyTimeSuffixes,
    kPrettyBytesMetricSuffixes,
    kPrettyBytesBinarySuffixes,
    kPrettyBytesBinaryIECSuffixes,
    kPrettyUnitsMetricSuffixes,
    kPrettyUnitsBinarySuffixes,
    kPrettyUnitsBinaryIECSuffixes,
    kPrettySISuffixes,
};

}  // anonymouse namespace


std::string prettyPrint(double val, PrettyType type, bool addSpace) 
{
    char buf[100];

    // pick the suffixes to use
    assert(type >= 0);
    assert(type < PRETTY_NUM_TYPES);
    const PrettySuffix* suffixes = kPrettySuffixes[type];

    // find the first suffix we're bigger than -- then use it
    double abs_val = fabs(val);
    for (int i = 0; suffixes[i].suffix; ++i)
    {
        if (abs_val >= suffixes[i].val)
        {
            snprintf(buf, sizeof buf, "%.4g%s%s",
                (suffixes[i].val ? (val / suffixes[i].val)
                : val),
                (addSpace ? " " : ""),
                suffixes[i].suffix);
            return std::string(buf);
        }
    }

    // no suffix, we've got a tiny value -- just print it in sci-notation
    snprintf(buf, sizeof buf, "%.4g", val);
    return std::string(buf);
}

//TODO:
//1) Benchmark & optimize
double prettyToDouble(StringPiece *const prettyString, 
                      const PrettyType type) 
{
    double value = to<double>(prettyString);
    while (prettyString->size() > 0 && std::isspace(prettyString->front()))
    {
        prettyString->advance(1); //Skipping spaces between number and suffix
    }
    const PrettySuffix* suffixes = kPrettySuffixes[type];
    int longestPrefixLen = -1;
    int bestPrefixId = -1;
    for (int j = 0; suffixes[j].suffix; ++j)
    {
        if (suffixes[j].suffix[0] == ' ') //Checking for " " -> number rule.
        {
            if (longestPrefixLen == -1)
            {
                longestPrefixLen = 0; //No characters to skip
                bestPrefixId = j;
            }
        }
        else if (prettyString->startsWith(suffixes[j].suffix))
        {
            int suffixLen = strlen(suffixes[j].suffix);
            //We are looking for a longest suffix matching prefix of the string
            //after numeric value. We need this in case suffixes have common prefix.
            if (suffixLen > longestPrefixLen)
            {
                longestPrefixLen = suffixLen;
                bestPrefixId = j;
            }
        }
    }
    if (bestPrefixId == -1)  //No valid suffix rule found
    {
        throw std::invalid_argument(to<std::string>(
            "Unable to parse suffix \"",
            prettyString->toString(), "\""));
    }
    prettyString->advance(longestPrefixLen);
    return suffixes[bestPrefixId].val ? value * suffixes[bestPrefixId].val :
        value;
}

double prettyToDouble(StringPiece prettyString, const PrettyType type)
{
    double result = prettyToDouble(&prettyString, type);
    detail::enforceWhitespace(prettyString.data(),
        prettyString.data() + prettyString.size());
    return result;
}
