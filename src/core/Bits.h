/*
 * Copyright 2014 Facebook, Inc.
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

/**
 * Various low-level, bit-manipulation routines.
 *
 * findFirstSet(x)  [constexpr]
 *    find first (least significant) bit set in a value of an integral type,
 *    1-based (like ffs()).  0 = no bits are set (x == 0)
 *
 * findLastSet(x)  [constexpr]
 *    find last (most significant) bit set in a value of an integral type,
 *    1-based.  0 = no bits are set (x == 0)
 *    for x != 0, findLastSet(x) == 1 + floor(log2(x))
 *
 * nextPowTwo(x)  [constexpr]
 *    Finds the next power of two >= x.
 *
 * isPowTwo(x)  [constexpr]
 *    return true iff x is a power of two
 *
 * popcount(x)
 *    return the number of 1 bits in x
 *
 * Endian
 *    convert between native, big, and little endian representation
 *    Endian::big(x)      big <-> native
 *    Endian::little(x)   little <-> native
 *    Endian::swap(x)     big <-> little
 *
 * @author Tudor Bosman (tudorb@fb.com)
 */
 
 #pragma once
 
#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include "Platform.h"

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <byteswap.h>
#endif



// Generate overloads for findFirstSet as wrappers around
// appropriate ffs, ffsl, ffsll gcc builtins
template <class T>
inline const typename std::enable_if<
    (std::is_integral<T>::value &&
    std::is_unsigned<T>::value &&
    sizeof(T) <= sizeof(uint32_t)),
unsigned int>::type
findFirstSet(T x) 
{
#ifdef _MSC_VER
    unsigned long index;
    return (_BitScanForward(&index, x) ? index + 1 : 0);
#else
    return __builtin_ffs(x);
#endif
}

template <class T>
inline const typename std::enable_if<
    (std::is_integral<T>::value &&
    std::is_unsigned<T>::value &&
    sizeof(T) > sizeof(uint32_t) &&
    sizeof(T) <= sizeof(uint64_t)),
unsigned int > ::type
findFirstSet(T x) 
{
#ifdef _MSC_VER
    unsigned long index;
    return (_BitScanForward64(&index, x) ? index + 1 : 0);
#else
    return __builtin_ffsll(x);
#endif
}

template <class T>
inline const typename std::enable_if<
    (std::is_integral<T>::value && std::is_signed<T>::value),
    unsigned int>::type
findFirstSet(T x) 
{
    // Note that conversion from a signed type to the corresponding unsigned
    // type is technically implementation-defined, but will likely work
    // on any impementation that uses two's complement.
    return findFirstSet(static_cast<typename std::make_unsigned<T>::type>(x));
}

// findLastSet: return the 1-based index of the highest bit set
// for x > 0, findLastSet(x) == 1 + floor(log2(x))
template <class T>
inline const typename std::enable_if<
    (std::is_integral<T>::value &&
    std::is_unsigned<T>::value &&
    sizeof(T) <= sizeof(uint32_t)),
    unsigned int>::type
findLastSet(T x) 
{
#ifdef _MSC_VER
    unsigned long index;
    return _BitScanReverse(&index, x) ? index + 1 : 0;
#else
    return x ? 8 * sizeof(uint32_t) - __builtin_clz(x) : 0;
#endif
}

template <class T>
inline const typename std::enable_if<
    (std::is_integral<T>::value &&
    std::is_unsigned<T>::value &&
    sizeof(T) > sizeof(uint32_t) &&
    sizeof(T) <= sizeof(uint64_t)),
    unsigned int > ::type
findLastSet(T x) 
{
#ifdef _MSC_VER
    unsigned long index;
    return _BitScanReverse64(&index, x) ? index + 1 : 0;
#else
    return x ? 8 * sizeof(uint64_t) - __builtin_clzll(x) : 0;
#endif
}


template <class T>
inline const typename std::enable_if<
    (std::is_integral<T>::value &&
    std::is_signed<T>::value),
    unsigned int>::type
findLastSet(T x) 
{
    return findLastSet(static_cast<typename std::make_unsigned<T>::type>(x));
}

template <class T>
inline const typename std::enable_if<
    std::is_integral<T>::value && std::is_unsigned<T>::value,
    T>::type
nextPowTwo(T v) 
{
    return v ? (1ul << findLastSet(v - 1)) : 1;
}

template <class T>
inline const typename std::enable_if<
    std::is_integral<T>::value && std::is_unsigned<T>::value,
    bool>::type
isPowTwo(T v) 
{
    return (v != 0) && !(v & (v - 1));
}

/**
 * Population count
 */
template <class T>
inline typename std::enable_if<
    (std::is_integral<T>::value &&
    std::is_unsigned<T>::value &&
    sizeof(T) <= sizeof(uint32_t)),
    size_t>::type
popcount(T x) 
{
#ifdef _MSC_VER
    return __popcnt(x);
#else
    return __builtin_popcount(x);
#endif
}


template <class T>
inline typename std::enable_if<
    (std::is_integral<T>::value &&
    std::is_unsigned<T>::value &&
    sizeof(T) > sizeof(uint32_t) &&
    sizeof(T) <= sizeof(uint64_t)),
    size_t > ::type
popcount(T x) 
{
#ifdef _MSC_VER
    return __popcnt64(x);
#else
    return __builtin_popcountll(x);
#endif
}


/**
* Endianness detection and manipulation primitives.
*/
namespace detail {

template <class T>
struct EndianIntBase 
{
public:
    static T swap(T x);
};


#define FB_GEN(t, fn) \
template<> inline t EndianIntBase<t>::swap(t x) { return fn(x); }

// fn(x) expands to (x) if the second argument is empty, which is exactly
// what we want for [u]int8_t. Also, gcc 4.7 on Intel doesn't have
// __builtin_bswap16 for some reason, so we have to provide our own.
FB_GEN(int8_t, )
FB_GEN(uint8_t, )

#ifdef _MSC_VER
FB_GEN(int64_t, _byteswap_uint64)
FB_GEN(uint64_t, _byteswap_uint64)
FB_GEN(int32_t, _byteswap_ulong)
FB_GEN(uint32_t, _byteswap_ulong)
FB_GEN(int16_t, _byteswap_ushort)
FB_GEN(uint16_t, _byteswap_ushort)
#else
FB_GEN(int64_t, __builtin_bswap64)
FB_GEN(uint64_t, __builtin_bswap64)
FB_GEN(int32_t, __builtin_bswap32)
FB_GEN(uint32_t, __builtin_bswap32)
FB_GEN(int16_t, our_bswap16)
FB_GEN(uint16_t, our_bswap16)
#endif

#undef FB_GEN


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

template <class T>
struct EndianInt : public detail::EndianIntBase < T > 
{
public:
    static T big(T x) { return EndianInt::swap(x); }
    static T little(T x) { return x; }
};

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

template <class T>
struct EndianInt : public detail::EndianIntBase < T > 
{
public:
    static T big(T x) { return x; }
    static T little(T x) { return EndianInt::swap(x); }
};
#else
# error Your machine uses a weird endianness!
#endif  /* __BYTE_ORDER__ */

}  // namespace detail


// big* convert between native and big-endian representations
// little* convert between native and little-endian representations
// swap* convert between big-endian and little-endian representations
//
// ntohs, htons == big16
// ntohl, htonl == big32
#define FB_GEN1(fn, t, sz) \
  static t fn##sz(t x) { return fn<t>(x); } \

#define FB_GEN2(t, sz) \
  FB_GEN1(swap, t, sz) \
  FB_GEN1(big, t, sz) \
  FB_GEN1(little, t, sz)

#define FB_GEN(sz) \
  FB_GEN2(uint##sz##_t, sz) \
  FB_GEN2(int##sz##_t, sz)


class Endian 
{
public:
    enum class Order : uint8_t 
    {
        LITTLE,
        BIG
    };

    static const Order order =
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        Order::LITTLE;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        Order::BIG;
#else
# error Your machine uses a weird endianness!
#endif  /* __BYTE_ORDER__ */

    template <class T> static T swap(T x) 
    {
        return detail::EndianInt<T>::swap(x);
    }
    template <class T> static T big(T x) 
    {
        return detail::EndianInt<T>::big(x);
    }
    template <class T> static T little(T x) 
    {
        return detail::EndianInt<T>::little(x);
    }

    FB_GEN(64)
    FB_GEN(32)
    FB_GEN(16)
    FB_GEN(8)
};

#undef FB_GEN
#undef FB_GEN2
#undef FB_GEN1


template <class T, class Enable = void> struct Unaligned;

/**
 * Representation of an unaligned value of a POD type.
 */
FOLLY_PACK_PUSH
template <class T>
struct Unaligned<
    T,
    typename std::enable_if<std::is_pod<T>::value>::type> 
{
    Unaligned() = default;  // uninitialized
    /* implicit */ Unaligned(T v) : value(v) { }
    T value;
} FOLLY_PACK_ATTR;
FOLLY_PACK_POP

/**
 * Read an unaligned value of type T and return it.
 */
template <class T>
inline T loadUnaligned(const void* p) 
{
    static_assert(sizeof(Unaligned<T>) == sizeof(T), "Invalid unaligned size");
    static_assert(alignof(Unaligned<T>) == 1, "Invalid alignment");
    return static_cast<const Unaligned<T>*>(p)->value;
}

/**
 * Write an unaligned value of type T.
 */
template <class T>
inline void storeUnaligned(void* p, T value) 
{
    static_assert(sizeof(Unaligned<T>) == sizeof(T), "Invalid unaligned size");
    static_assert(alignof(Unaligned<T>) == 1, "Invalid alignment");
    new (p)Unaligned<T>(value);
}