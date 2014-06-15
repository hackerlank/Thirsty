#pragma once

#include <cstdint>

enum
{
    MAX_BODY_LEN = 1024 * 1024,     // 1M
};

struct Header
{
    uint32_t     size;              // ����
    uint32_t     size_checksum;     // ����CRC
    uint32_t     body_checksum;     // ����CRC
};
