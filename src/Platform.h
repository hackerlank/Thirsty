//
//
//

#pragma once

// portable version check
#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
#  define __GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= \
                                   ((maj) << 16) + (min))
# else
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

// detection for 64 bit
#if defined(__x86_64__) || defined(_M_X64)
# define FOLLY_X64  1
#else
# define FOLLY_X64  0
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif

#ifdef _MSC_VER
#define ALIGN(x)        __declspec(align(x))
#elif defined(__GUNC__)
#define ALIGN(x)        __attribute__((aligned(x)))
#else
#error "ALIGN() not supported on this platform"
#endif

// compiler specific attribute translation
// msvc should come first, so if clang is in msvc mode it gets the right defines

// NOTE: this will only do checking in msvc with versions that support /analyze
#if _MSC_VER
# ifdef _USE_ATTRIBUTES_FOR_SAL
#    undef _USE_ATTRIBUTES_FOR_SAL
# endif
# define _USE_ATTRIBUTES_FOR_SAL 1
# include <sal.h>
# define FOLLY_PRINTF_FORMAT _Printf_format_string_
# define FOLLY_PRINTF_FORMAT_ATTR(format_param, dots_param) /**/
#else
# define FOLLY_PRINTF_FORMAT /**/
# define FOLLY_PRINTF_FORMAT_ATTR(format_param, dots_param) \
  __attribute__((format(printf, format_param, dots_param)))
#endif

/* Platform specific TLS support
 * gcc implements __thread
 * msvc implements __declspec(thread)
 * the semantics are the same (but remember __thread is broken on apple)
 */
#if defined(_MSC_VER)
# define FOLLY_TLS __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
# define FOLLY_TLS __thread
#else
# error cannot define platform specific thread local storage
#endif

// MSVC specific defines
// mainly for posix compat
#ifdef _MSC_VER

// this definition is in a really silly place with a silly name
// and ifdefing it every time we want it is painful
#include <basetsd.h>
typedef SSIZE_T ssize_t;

// sprintf semantics are not exactly identical
// but current usage is not a problem
# define snprintf   _snprintf

// semantics here are identical
# define strerror_r(errno,buf,len) strerror_s(buf,len,errno)

// compiler specific to compiler specific
# define __PRETTY_FUNCTION__    __FUNCSIG__
#endif
