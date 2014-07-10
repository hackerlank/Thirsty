#pragma once

#include <cstdint>


struct Header
{
    uint32_t     size;              // ����
    uint32_t     size_crc;          // ����CRC
    uint32_t     body_crc;          // ����CRC
};

enum
{
    MAX_BODY_LEN = (1024 * 1024) - sizeof(Header),     // max buffer size
};
