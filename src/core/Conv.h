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
 * Converts anything to anything, with an emphasis on performance and
 * safety.
 *
 * @author Andrei Alexandrescu (andrei.alexandrescu@fb.com)
 */

#ifndef FOLLY_BASE_CONV_H_
#define FOLLY_BASE_CONV_H_

#include "Preprocessor.h"
#include "Range.h"

#include <boost/implicit_cast.hpp>
#include <type_traits>
#include <limits>
#include <string>
#include <tuple>
#include <stdexcept>
#include <typeinfo>
#include <limits.h>

// V8 JavaScript implementation
#include <double-conversion.h>

#define FOLLY_RANGE_CHECK(condition, message)                           \
  ((condition) ? (void)0 : throw std::range_error(                      \
    (__FILE__ "(" + std::to_string((long long int) __LINE__) + "): "    \
     + (message)).c_str()))


/*******************************************************************************
 * Integral to integral
 ******************************************************************************/

/**
 * Checked conversion from integral to integral. The checks are only
 * performed when meaningful, e.g. conversion from int to long goes
 * unchecked.
 */
template <class Tgt, class Src>
typename std::enable_if<
  std::is_integral<Src>::value && std::is_integral<Tgt>::value,
  Tgt>::type
to(const Src & value) {
  /* static */ if (std::numeric_limits<Tgt>::max()
                   < std::numeric_limits<Src>::max()) {
    FOLLY_RANGE_CHECK(value <= std::numeric_limits<Tgt>::max(),
      "Overflow"
    );
  }
  /* static */ if (std::is_signed<Src>::value &&
                   (!std::is_signed<Tgt>::value || sizeof(Src) > sizeof(Tgt))) {
    FOLLY_RANGE_CHECK(value >= std::numeric_limits<Tgt>::min(),
      "Negative overflow"
    );
  }
  return static_cast<Tgt>(value);
}

/*******************************************************************************
 * Floating point to floating point
 ******************************************************************************/

template <class Tgt, class Src>
typename std::enable_if<
  std::is_floating_point<Tgt>::value && std::is_floating_point<Src>::value,
  Tgt>::type
to(const Src & value) {
  /* static */ if (std::numeric_limits<Tgt>::max() <
                   std::numeric_limits<Src>::max()) {
    FOLLY_RANGE_CHECK(value <= std::numeric_limits<Tgt>::max(),
                      "Overflow");
    FOLLY_RANGE_CHECK(value >= -std::numeric_limits<Tgt>::max(),
                      "Negative overflow");
  }
  return boost::implicit_cast<Tgt>(value);
}


/*******************************************************************************
 * Conversions from integral types to string types.
 ******************************************************************************/

/**
 * Returns the number of digits in the base 10 representation of an
 * uint64_t. Useful for preallocating buffers and such. It's also used
 * internally, see below. Measurements suggest that defining a
 * separate overload for 32-bit integers is not worthwhile.
 */

inline uint32_t digits10(uint64_t v) {
    uint32_t result = 1;
    for (;;) {
        if (v < 10) return result;
        if (v < 100) return result + 1;
        if (v < 1000) return result + 2;
        if (v < 10000) return result + 3;
        // Skip ahead by 4 orders of magnitude
        v /= 10000U;
        result += 4;
    }
}

/**
 * Copies the ASCII base 10 representation of v into buffer and
 * returns the number of bytes written. Does NOT append a \0. Assumes
 * the buffer points to digits10(v) bytes of valid memory. Note that
 * uint64 needs at most 20 bytes, uint32_t needs at most 10 bytes,
 * uint16_t needs at most 5 bytes, and so on. Measurements suggest
 * that defining a separate overload for 32-bit integers is not
 * worthwhile.
 *
 * This primitive is unsafe because it makes the size assumption and
 * because it does not add a terminating \0.
 */

inline uint32_t uint64ToBufferUnsafe(uint64_t v, char *const buffer) {
    auto const result = digits10(v);
    // WARNING: using size_t or pointer arithmetic for pos slows down
    // the loop below 20x. This is because several 32-bit ops can be
    // done in parallel, but only fewer 64-bit ones.
    uint32_t pos = result - 1;
    while (v >= 10) {
        // Keep these together so a peephole optimization "sees" them and
        // computes them in one shot.
        auto const q = v / 10;
        auto const r = static_cast<uint32_t>(v % 10);
        buffer[pos--] = '0' + r;
        v = q;
    }
    // Last digit is trivial to handle
    buffer[pos] = static_cast<uint32_t>(v) + '0';
    return result;
}

/**
 * A single char gets appended.
 */
template <class Tgt>
void toAppend(Tgt* result, char value) {
    *result += value;
}

/**
 * Ubiquitous helper template for writing string appenders
 */
template <class T> struct IsSomeString {
    enum {
        value = std::is_same<T, std::string>::value
    };
};

/**
 * Everything implicitly convertible to const char* gets appended.
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_convertible<Src, const char*>::value
    && IsSomeString<Tgt>::value>::type
toAppend(Tgt* result, Src value) {
    // Treat null pointers like an empty string, as in:
    // operator<<(std::ostream&, const char*).
    const char* c = value;
    if (c) {
        result->append(value);
    }
}

/**
 * Strings get appended, too.
 */
template <class Tgt, class Src>
typename std::enable_if<
    IsSomeString<Src>::value && IsSomeString<Tgt>::value>::type
toAppend(Tgt* result, const Src& value) {
    result->append(value);
}

/**
 * and StringPiece objects too
 */
template <class Tgt>
typename std::enable_if<
    IsSomeString<Tgt>::value>::type
toAppend(Tgt* result, StringPiece value) {
    result->append(value.data(), value.size());
}

/**
 * int32_t and int64_t to string (by appending) go through here. The
 * result is APPENDED to a preexisting string passed as the second
 * parameter. This should be efficient with fbstring because fbstring
 * incurs no dynamic allocation below 23 bytes and no number has more
 * than 22 bytes in its textual representation (20 for digits, one for
 * sign, one for the terminating 0).
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_integral<Src>::value && std::is_signed<Src>::value &&
    IsSomeString<Tgt>::value && sizeof(Src) >= 4>::type
toAppend(Tgt* result, Src value) {
    char buffer[20];
    if (value < 0) {
        result->push_back('-');
        result->append(buffer, uint64ToBufferUnsafe(-value, buffer));
    }
    else {
        result->append(buffer, uint64ToBufferUnsafe(value, buffer));
    }
}

/**
 * As above, but for uint32_t and uint64_t.
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_integral<Src>::value && !std::is_signed<Src>::value
    && IsSomeString<Tgt>::value && sizeof(Src) >= 4>::type
toAppend(Tgt* result, Src value) {
    char buffer[20];
    result->append(buffer, buffer + uint64ToBufferUnsafe(value, buffer));
}

/**
 * All small signed and unsigned integers to string go through 32-bit
 * types int32_t and uint32_t, respectively.
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_integral<Src>::value
    && IsSomeString<Tgt>::value && sizeof(Src) < 4>::type
toAppend(Tgt* result, Src value) {
    typedef typename
        std::conditional<std::is_signed<Src>::value, int64_t, uint64_t>::type
        Intermediate;
    toAppend<Tgt>(static_cast<Intermediate>(value), result);
}

/**
 * Enumerated values get appended as integers.
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_enum<Src>::value && IsSomeString<Tgt>::value>::type
toAppend(Tgt* result, Src value) {
    toAppend(
        static_cast<typename std::underlying_type<Src>::type>(value), result);
}

/*******************************************************************************
 * Conversions from floating-point types to string types.
 ******************************************************************************/

/** Wrapper around DoubleToStringConverter **/
template <class Tgt, class Src>
typename std::enable_if<
    std::is_floating_point<Src>::value
    && IsSomeString<Tgt>::value>::type
toAppend(
  Src value,
  Tgt* result,
  double_conversion::DoubleToStringConverter::DtoaMode mode,
  unsigned int numDigits) {
    using namespace double_conversion;
    DoubleToStringConverter
        conv(DoubleToStringConverter::NO_FLAGS,
        "infinity", "NaN", 'E',
        -6,  // decimal in shortest low
        21,  // decimal in shortest high
        6,   // max leading padding zeros
        1);  // max trailing padding zeros
    char buffer[256];
    StringBuilder builder(buffer, sizeof(buffer));
    switch (mode) {
    case DoubleToStringConverter::SHORTEST:
        conv.ToShortest(value, &builder);
        break;
    case DoubleToStringConverter::FIXED:
        conv.ToFixed(value, numDigits, &builder);
        break;
    default:
        CHECK(mode == DoubleToStringConverter::PRECISION);
        conv.ToPrecision(value, numDigits, &builder);
        break;
    }
    const size_t length = builder.position();
    builder.Finalize();
    result->append(buffer, length);
}

/**
 * As above, but for floating point
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_floating_point<Src>::value
    && IsSomeString<Tgt>::value>::type
toAppend(Tgt* result, Src value) {
    toAppend(
        value, result, double_conversion::DoubleToStringConverter::SHORTEST, 0);
}

/**
 * Variadic conversion to string. Appends each element in turn.
 */
template <class Tgt, class Head, class... Tail>
typename std::enable_if<sizeof...(Tail) >= 2
    && IsSomeString<Tgt>::value>::type
toAppend(Tgt* result, const Head& v, const Tail&... vs) {
    toAppend(result, v);
    toAppend(result, vs...);
}

/**
 * Variadic base case: do nothing.
 */
template <class Tgt>
typename std::enable_if<IsSomeString<Tgt>::value>::type
toAppend(Tgt* result) {
}

#endif /* FOLLY_BASE_CONV_H_ */
