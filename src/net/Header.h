#pragma once

#include <cstdint>
#include <array>

struct Header
{
    uint32_t     size;              // 长度
    uint32_t     size_crc;          // 长度CRC
    uint32_t     body_crc;          // 内容CRC
};

enum
{
    MAX_BODY_LEN = (1024 * 1024) - sizeof(Header),     // max buffer size

    STACK_BUF_SIZE = 64,
};

typedef std::array<uint8_t, STACK_BUF_SIZE> StackBuffer;
