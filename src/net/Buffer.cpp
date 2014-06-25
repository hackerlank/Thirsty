#include "Buffer.h"
#include <cassert>
#include <cstring>   // memcpy
#include <zlib.h>


//////////////////////////////////////////////////////////////////////////
Buffer::Buffer()
    : data_(sizeof(Header)+RESERVED_SIZE, '0')
{
    Header* head = header();
    head->size = RESERVED_SIZE;
    make_head_checksum();
    make_body_checksum();
}

Buffer::Buffer(const void* data, size_t size)
    : data_(sizeof(Header)+size, '0')
{
    assert(data && size);
    Header* head = header();
    head->size = size;
    memcpy(body(), data, size);
    make_head_checksum();
    make_body_checksum();
}

Buffer::Buffer(Buffer&& other)
    : data_(std::move(other.data_))
{
}

Buffer::~Buffer()
{
}


void Buffer::reserve_body(size_t bodysize)
{
    if (bodysize > body_size())
    {
        data_.resize(bodysize);
    }
}

void Buffer::make_head_checksum()
{
    Header* head = header();
    head->size_crc = crc32(0, (const Bytef*)&head->size, sizeof(head->size));
}

void Buffer::make_body_checksum()
{
    Header* head = header();
    head->body_crc = crc32(0, (const Bytef*)body(), head->size);
}

bool Buffer::check_head_crc() const
{
    const Header* head = header();
    auto checksum = crc32(0, (const Bytef*)&head->size, sizeof(head->size));
    return (checksum == head->size_crc);
}

bool Buffer::check_body_crc() const
{
    const Header* head = header();
    auto checksum = crc32(0, (const Bytef*)body(), head->size);
    return head->body_crc == checksum;
}
