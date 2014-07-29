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

#include "Checksum.h"
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <boost/crc.hpp>
#include "Platform.h"
#include "core/CpuId.h"


namespace detail {

#if FOLLY_X64 

#ifdef _MSC_VER

#include <nmmintrin.h>
#define crc32c_u8       _mm_crc32_u8
#define crc32c_u16      _mm_crc32_u16
#define crc32c_u32      _mm_crc32_u32
#define crc32c_u64      _mm_crc32_u64

#elif __GNUC_PREREQ(4, 7)

#define crc32c_u8       __builtin_ia32_crc32qi
#define crc32c_u16      __builtin_ia32_crc32hi
#define crc32c_u32      __builtin_ia32_crc32si
#define crc32c_u64      __builtin_ia32_crc32di

#else
#error "no hardware-accelerated CRC-32C"
#endif

// Fast SIMD implementation of CRC-32C for x86 with SSE 4.2
    uint32_t crc32c_hw(const void* input, size_t nbytes, uint32_t crc)
{
    assert(input != nullptr);
    const uint8_t* data = (const uint8_t*)input;
    uint32_t sum = crc;
    size_t offset = 0;

    // Process bytes one at a time until we reach an 8-byte boundary and can
    // start doing aligned 64-bit reads.
    static uintptr_t ALIGN_MASK = sizeof(uint64_t)-1;
    size_t mask = (size_t)((uintptr_t)data & ALIGN_MASK);
    if (mask != 0)
    {
        size_t limit = std::min(nbytes, sizeof(uint64_t)-mask);
        while (offset < limit)
        {
            sum = (uint32_t)crc32c_u8(sum, data[offset]);
            offset++;
        }
    }

    // Process 8 bytes at a time until we have fewer than 8 bytes left.
    while (offset + sizeof(uint64_t) <= nbytes)
    {
        const uint64_t* src = (const uint64_t*)(data + offset);
        sum = (uint32_t)crc32c_u64(sum, *src);
        offset += sizeof(uint64_t);
    }

    // Process any bytes remaining after the last aligned 8-byte block.
    while (offset < nbytes)
    {
        sum = (uint32_t)crc32c_u8(sum, data[offset]);
        offset++;
    }
    return sum;
}

bool crc32c_hw_supported() 
{
    static CpuId id;
    return id.sse42();
}

#else

uint32_t crc32c_hw(const void* data, size_t nbytes, uint32_t crc)
{
    throw std::runtime_error("crc32_hw is not implemented on this platform");
}

bool crc32c_hw_supported() 
{
    return false;
}

#endif

uint32_t crc32c_sw(const void* input, size_t nbytes, uint32_t crc)
{
    assert(input != nullptr);
    const uint8_t* data = (const uint8_t*)input;
    // Reverse the bits in the starting checksum so they'll be in the
    // right internal format for Boost's CRC engine.
    //     O(1)-time, branchless bit reversal algorithm from
    //     http://graphics.stanford.edu/~seander/bithacks.html
    crc = ((crc >> 1) & 0x55555555) | ((crc & 0x55555555) << 1);
    crc = ((crc >> 2) & 0x33333333) | ((crc & 0x33333333) << 2);
    crc = ((crc >> 4) & 0x0f0f0f0f) | ((crc & 0x0f0f0f0f) << 4);
    crc = ((crc >> 8) & 0x00ff00ff) | ((crc & 0x00ff00ff) << 8);
    crc = (crc >> 16) | (crc << 16);

    static const uint32_t CRC32C_POLYNOMIAL = 0x1EDC6F41;
    boost::crc_optimal<32, CRC32C_POLYNOMIAL, ~0U, 0, true, true> sum(crc);
    sum.process_bytes(data, nbytes);
    return sum.checksum();
}

} // namespace detail

uint32_t crc32c(const void* data, size_t nbytes, uint32_t startingChecksum)
{
    assert(data != nullptr);
    if (detail::crc32c_hw_supported())
    {
        return detail::crc32c_hw(data, nbytes, startingChecksum);
    }
    else
    {
        return detail::crc32c_sw(data, nbytes, startingChecksum);
    }
}
