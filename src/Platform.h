#pragma once


#ifdef _MSC_VER   // Visual C++ 2013 or later
#define LIKELY(x)       (x)
#define UNLIKELY(x)     (x)
#define ALIGN(x)        __declspec(align(x))
#define THREAD_LOCAL    __declspec(thread)
#define EXPORT          __declspec(dllexport)
#define IMPORT          __declspec(dllimport)

#define snprintf        sprintf_s

#elif defined(__GNUC__)   // GNU C++ 4.7 or later
#define _NOEXCEPT       noexcept
#define _NOEXCEPT_OP(x) noexcept(x)
#define LIKELY(x)       (__builtin_expect((x), 1))
#define UNLIKELY(x)     (__builtin_expect((x), 0))
#define ALIGN(x)        __attribute__((aligned(x)))
#define THREAD_LOCAL    __thread
#define EXPORT          __attribute__ ((visibility("default")))
#define IMPORT

#else

#error "Unsupported platform!"

#endif
