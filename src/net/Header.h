#pragma once

#include <cstdint>

enum
{
    MAX_BODY_LEN = 1024 * 1024,     // 1M
};

struct Header
{
    uint32_t     size;              // 长度
    uint32_t     size_checksum;     // 长度CRC
    uint32_t     body_checksum;     // 内容CRC
};
