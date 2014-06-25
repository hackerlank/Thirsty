#pragma once


#ifdef _MSC_VER   // Visual C++

#define ALIGN(x)        __declspec(align(x))
#define THREAD_LOCAL    __declspec(thread)
#define EXPORT          __declspec(dllexport)
#define IMPORT          __declspec(dllimport)

#define snprintf        sprintf_s

#elif defined(__GNUC__)   // GNU C++

#define ALIGN(x)        __attribute__((aligned(x)))
#define THREAD_LOCAL    __thread
#define EXPORT          __attribute__ ((visibility("default")))
#define IMPORT

#else

#error "Unsupported platform!"

#endif