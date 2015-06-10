/*
 * Copyright 2015 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <type_traits>

#ifdef __APPLE__
# include <malloc/malloc.h>
#endif

// MaxAlign: std::max_align_t isn't supported by gcc
#ifdef __GNUC__
struct MaxAlign { char c; } __attribute__((__aligned__));
#else /* !__GNUC__ */
#endif

// detection for 64 bit
#if defined(__x86_64__) || defined(_M_X64)
# define FOLLY_X64  1
#else
# define FOLLY_X64  0
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
# define LIKELY(x)   (__builtin_expect((x), 1))
# define UNLIKELY(x) (__builtin_expect((x), 0))
#else
# define LIKELY(x)   (x)
# define UNLIKELY(x) (x)
#endif

#if defined(_MSC_VER)
# define FOLLY_ALIGN(x) __declspec(align(x))
#else
# define FOLLY_ALIGN(x) __attribute__((aligned(x)))
#endif

// deprecated
#if defined(__clang__) || defined(__GNUC__)
# define FOLLY_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#else
# define FOLLY_DEPRECATED
#endif

// noreturn
#if defined(_MSC_VER)
# define FOLLY_NORETURN __declspec(noreturn)
#elif defined(__clang__) || defined(__GNUC__)
# define FOLLY_NORETURN __attribute__((__noreturn__))
#else
# define FOLLY_NORETURN
#endif

// noinline
#ifdef _MSC_VER
# define FOLLY_NOINLINE __declspec(noinline)
#elif defined(__clang__) || defined(__GNUC__)
# define FOLLY_NOINLINE __attribute__((__noinline__))
#else
# define FOLLY_NOINLINE
#endif

// always inline
#ifdef _MSC_VER
# define FOLLY_ALWAYS_INLINE __forceinline
#elif defined(__clang__) || defined(__GNUC__)
# define FOLLY_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
# define FOLLY_ALWAYS_INLINE
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

// portable version check
#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
/* nolint */
#  define __GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= \
                                   ((maj) << 16) + (min))
# else
/* nolint */
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

/* Define macro wrappers for C++11's "final" and "override" keywords, which
 * are supported in gcc 4.7 but not gcc 4.6. */
#if !defined(FOLLY_FINAL) && !defined(FOLLY_OVERRIDE)
# if defined(__clang__) || __GNUC_PREREQ(4, 7)
#  define FOLLY_FINAL final
#  define FOLLY_OVERRIDE override
# else
#  define FOLLY_FINAL /**/
#  define FOLLY_OVERRIDE /**/
# endif
#endif

/* Platform specific TLS support
 * gcc implements __thread
 * msvc implements __declspec(thread)
 * the semantics are the same
 * (but remember __thread has different semantics when using emutls (ex. apple))
 */
#if defined(_MSC_VER)
# define FOLLY_TLS __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
# define FOLLY_TLS __thread
#else
# error cannot define platform specific thread local storage
#endif

/* Define attribute wrapper for function attribute used to disable
 * address sanitizer instrumentation. Unfortunately, this attribute
 * has issues when inlining is used, so disable that as well. */
#ifdef FOLLY_SANITIZE_ADDRESS
# if defined(__clang__)
#  if __has_attribute(__no_address_safety_analysis__)
#   define FOLLY_DISABLE_ADDRESS_SANITIZER \
      __attribute__((__no_address_safety_analysis__, __noinline__))
#  elif __has_attribute(__no_sanitize_address__)
#   define FOLLY_DISABLE_ADDRESS_SANITIZER \
      __attribute__((__no_sanitize_address__, __noinline__))
#  endif
# elif defined(__GNUC__)
#  define FOLLY_DISABLE_ADDRESS_SANITIZER \
     __attribute__((__no_address_safety_analysis__, __noinline__))
# endif
#endif
#ifndef FOLLY_DISABLE_ADDRESS_SANITIZER
# define FOLLY_DISABLE_ADDRESS_SANITIZER
#endif

#if !defined(__clang__) && !defined(_MSC_VER)
#define FOLLY_CONSTEXPR constexpr
#else
// GCC is the only compiler with intrinsics constexpr.
#define FOLLY_CONSTEXPR const
#endif

// MSVC specific defines
// mainly for posix compat
#ifdef _MSC_VER

#include <intrin.h>

// this definition is in a really silly place with a silly name
// and ifdefing it every time we want it is painful
#include <basetsd.h>
typedef SSIZE_T ssize_t;

// Windows use LLP64
#ifndef __SIZEOF_LONG__
#define __SIZEOF_LONG__     4
#endif

// sprintf semantics are not exactly identical
// but current usage is not a problem
# define snprintf   sprintf_s

// semantics here are identical
# define strerror_r(errno,buf,len) strerror_s(buf,len,errno)

// compiler specific to compiler specific
# define __PRETTY_FUNCTION__    __FUNCSIG__

// alignof keyword
#define alignof     __alignof

// noexcept operator was not supported
#define noexcept    _NOEXCEPT

#define constexpr   

#endif // _MSC_VER
