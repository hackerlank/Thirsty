#pragma once


#ifdef _WIN32   // Windows platform

#define ALIGN(x)        __declspec(align(x))
#define THREAD_LOCAL    __declspec(thread)
#define EXPORT          __declspec(dllexport)
#define IMPORT          __declspec(dllimport)

#elif defined(__linux__) && defined(__GUNC__)   // linux platform

#define ALIGN(x)        __attribute__((aligned(x)))
#define THREAD_LOCAL    __thread
#define EXPORT          __attribute__ ((visibility("default")))
#define IMPORT

#else

#error "Unsupported platform!"

#endif