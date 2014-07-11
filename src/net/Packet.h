#pragma once

#include <cstdint>
#include <array>

/**
 *  a network packet on the wire consist of :
 *
 *  +---------------+----------------------+
 *  |    Header     |     content          |
 *  +---------------+----------------------+
 *
 */

struct Header
{
    uint32_t     size;              // content size
    uint32_t     size_checksum;     // checksum value of size
    uint32_t     content_checksum;  // checksum value of content
};

enum
{
    // max content size, size of a packet is limited to 1M
    MAX_CONTENT_LEN = (1024 * 1024) - sizeof(Header),

    // small buffer pre-allocated for every connection,
    // reduce frequency of memory allocating
    STACK_BUF_SIZE = 64,
};

typedef std::array<uint8_t, STACK_BUF_SIZE> StackBuffer;
