#include "Buffer.h"
#include <cassert>
#include <zlib.h>


// todo: replace alloc and free routine with jemalloc or tcmalloc
static char* pool_alloc(size_t size)
{
    return (char*)malloc(size);
}

static void pool_free(void* data)
{
    free(data);
}


//////////////////////////////////////////////////////////////////////////
Buffer::Buffer()
{
    memcpy(&head_, 0, sizeof(head_));
    body_ = nullptr;
}

Buffer::Buffer(const void* data, size_t size)
    : body_(pool_alloc(size))
{
    assert(data && size);
    memcpy(body_, data, size);
    head_.size = size;
    make_head_checksum();
    make_body_checksum();
}

Buffer::Buffer(Buffer&& other)
    : body_(other.body_)
{    
    memcpy(&head_, &other.head_, sizeof(head_));
    memset(&other.head_, 0, sizeof(other.head_));
    other.body_ = nullptr;
}

Buffer::~Buffer()
{
    pool_free(body_);
    body_ = nullptr;
}

void Buffer::resize(size_t size)
{
    pool_free(body_);
    head_.size = size;
    head_.body_crc = 0;
    body_ = nullptr;
    if (size > 0)
    {
        body_ = pool_alloc(size);
        make_head_checksum();
        make_body_checksum();
    }    
}

void Buffer::resize(const void* data, size_t size)
{
    assert(data && size);
    pool_free(body_);
    head_.size = size;
    body_ = pool_alloc(size);
    memcpy(body_, data, size);
    make_head_checksum();
    make_body_checksum();
}

void Buffer::make_head_checksum()
{
    head_.size_crc = crc32(0, (const Bytef*)&head_.size, sizeof(head_.size));
}

void Buffer::make_body_checksum()
{
    head_.body_crc = crc32(0, (const Bytef*)body_, head_.size);
}

bool Buffer::check_head_crc() const
{
    auto checksum = crc32(0, (const Bytef*)&head_.size, sizeof(head_.size));
    return (checksum == head_.size_crc);
}

bool Buffer::check_body_crc() const
{
    auto checksum = crc32(0, (const Bytef*)body_, head_.size);
    return head_.body_crc == checksum;
}
