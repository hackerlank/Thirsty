#include "Utils.h"
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include "Platform.h"
#include "logging.h"


#if defined(_MSC_VER)
#include <windows.h>
inline uint64_t getPerformanceFreqency()
{
    static LARGE_INTEGER freq;
    if (freq.QuadPart == 0)
    {
        CHECK(QueryPerformanceFrequency(&freq));
    }
    return freq.QuadPart;
}

uint64_t getNowTickCount()
{
    LARGE_INTEGER now;
    CHECK(QueryPerformanceCounter(&now));
    return (now.QuadPart * 1000000000UL) / getPerformanceFreqency();
}

#elif defined(__GNUC__)

uint64_t getNowTickCount()
{
    timespec ts;
    CHECK(clock_gettime(CLOCK_REALTIME, &ts) == 0);
    return (ts.tv_sec * 1000000000UL) + ts.tv_nsec;
}

#endif

std::string bin2hex(const void* input, size_t length)
{
    assert(input);
    std::string result;
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < length; i++)
    {
        uint8_t ch = ((uint8_t*)(input))[i];
        result += hex[ch >> 4];
        result += hex[ch & 0x0f];
    }
    return std::move(result);
}

int32_t GetFileLength(const char* filename)
{
    assert(filename);
    FILE* fp = fopen(filename, "r");
    if (fp)
    {
        int32_t length = 0;
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        return length;
    }
    return 0;
}

void LogFileM(const char* module, const char* fmt, ...)
{
    assert(module && fmt);
    time_t now = time(NULL);
    tm date = *localtime(&now);
    char filename[256];
    int r = snprintf(filename, 256, "%s_%d-%d-%d", module, date.tm_year + 1900,
        date.tm_mon, date.tm_mday);
    if (r <= 0)
    {
        return ;
    }
    char buffer[512];
    va_list ap;
    va_start(ap, fmt);
    r = vsnprintf(buffer, 512, fmt, ap);
    va_end(ap);
    if (r <= 0)
    {
        return ;
    }
    FILE* fp = fopen(filename, "a+");
    if (fp)
    {
        fwrite(buffer, 1, r, fp);
        fclose(fp);
    }
}
