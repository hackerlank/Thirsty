#pragma once

#include <cstdint>
#include <array>

struct Header
{
    uint32_t     size;              // ����
    uint32_t     size_crc;          // ����CRC
    uint32_t     body_crc;          // ����CRC
};

enum
{
    MAX_BODY_LEN = (1024 * 1024) - sizeof(Header),     // max buffer size

    STACK_BUF_SIZE = 64,
};

typedef std::array<uint8_t, STACK_BUF_SIZE> StackBuffer;
