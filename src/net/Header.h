#pragma once

#include <cstdint>


struct Header
{
    uint32_t     size;              // 长度
    uint32_t     size_crc;          // 长度CRC
    uint32_t     body_crc;          // 内容CRC
};

enum
{
    MAX_BODY_LEN = (1024 * 1024) - sizeof(Header),     // max buffer size
};
