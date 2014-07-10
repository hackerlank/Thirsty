//
//
//

#pragma once

#include <boost/predef.h>

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

// Define macro wrappers for C++11's "final" and "override" keywords
#if !defined(FOLLY_FINAL) && !defined(FOLLY_OVERRIDE)
# if defined(__clang__) || __GNUC_PREREQ(4, 7)
#  define FOLLY_FINAL final
#  define FOLLY_OVERRIDE override
# else
#  define FOLLY_FINAL /**/
#  define FOLLY_OVERRIDE /**/
# endif
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
# define LIKELY(x)   (__builtin_expect((x), 1))
# define UNLIKELY(x) (__builtin_expect((x), 0))
#else
# define LIKELY(x)   (x)
# define UNLIKELY(x) (x)
#endif

#ifdef _MSC_VER
# define ALIGN(x)        __declspec(align(x))
#else
# define ALIGN(x)        __attribute__((aligned(x)))
#endif

// compiler specific attribute translation
// msvc should come first, so if clang is in msvc mode it gets the right defines

// NOTE: this will only do checking in msvc with versions that support /analyze
#if _MSC_VER
# ifdef _USE_ATTRIBUTES_FOR_SAL
#   undef _USE_ATTRIBUTES_FOR_SAL
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

// packing is very ugly in msvc
#ifdef _MSC_VER
# define FOLLY_PACK_ATTR /**/
# define FOLLY_PACK_PUSH __pragma(pack(push, 1))
# define FOLLY_PACK_POP __pragma(pack(pop))
#elif defined(__clang__) || defined(__GNUC__)
# define FOLLY_PACK_ATTR __attribute__((packed))
# define FOLLY_PACK_PUSH /**/
# define FOLLY_PACK_POP /**/
#else
# define FOLLY_PACK_ATTR /**/
# define FOLLY_PACK_PUSH /**/
# define FOLLY_PACK_POP /**/
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
# define snprintf   sprintf_s

// semantics here are identical
# define strerror_r(errno,buf,len) strerror_s(buf,len,errno)

// compiler specific to compiler specific
# define __PRETTY_FUNCTION__    __FUNCSIG__

#define alignof     __alignof

// endian detection
#define __ORDER_LITTLE_ENDIAN__     1234
#define __ORDER_BIG_ENDIAN__        4321

#if BOOST_ENDIAN_LITTLE_BYTE == 1
#   define __BYTE_ORDER__  __ORDER_LITTLE_ENDIAN__
#elif BOOST_ENDIAN_BIG_BYTE == 1
#   define __BYTE_ORDER__  __ORDER_BIG_ENDIAN__
#endif

// noexcept operator was not supported in MSVC 2013
#define noexcept       _NOEXCEPT

#endif // _MSC_VER
