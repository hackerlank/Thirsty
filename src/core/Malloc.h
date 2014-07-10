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

#pragma once

#include <exception>

/**
* For jemalloc's size classes, see
* http://www.canonware.com/download/jemalloc/jemalloc-latest/doc/jemalloc.html
*/
inline size_t goodMallocSize(size_t minSize)
{
    if (minSize <= 64)
    {
        // Choose smallest allocation to be 64 bytes - no tripping over
        // cache line boundaries, and small string optimization takes care
        // of short strings anyway.
        return 64;
    }
    if (minSize <= 512)
    {
        // Round up to the next multiple of 64; we don't want to trip over
        // cache line boundaries.
        return (minSize + 63) & ~size_t(63);
    }
    if (minSize <= 3840)
    {
        // Round up to the next multiple of 256
        return (minSize + 255) & ~size_t(255);
    }
    if (minSize <= 4072 * 1024)
    {
        // Round up to the next multiple of 4KB
        return (minSize + 4095) & ~size_t(4095);
    }
    // Holy Moly
    // Round up to the next multiple of 4MB
    return (minSize + 4194303) & ~size_t(4194303);
}

/**
* Trivial wrappers around malloc, calloc, realloc that check for allocation
* failure and throw std::bad_alloc in that case.
*/
inline void* checkedMalloc(size_t size) 
{
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}

inline void* checkedCalloc(size_t n, size_t size) 
{
    void* p = calloc(n, size);
    if (!p) throw std::bad_alloc();
    return p;
}

inline void* checkedRealloc(void* ptr, size_t size) 
{
    void* p = realloc(ptr, size);
    if (!p) throw std::bad_alloc();
    return p;
}
