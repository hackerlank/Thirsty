#include "Buffer.h"
#include <cassert>
#include <cstring>   // memcpy



//////////////////////////////////////////////////////////////////////////
Buffer::Buffer(size_t size)
    : size_(size)
{
    assert(size);
    data_ = (uint8_t*)malloc(size);
}

Buffer::Buffer(const void* data, size_t size)
    : size_(size)
{
    assert(data && size);
    data_ = (uint8_t*)malloc(size);
    memcpy(data_, data, size);
}

Buffer::Buffer(Buffer&& other)
    : size_(other.size_),
      data_(other.data_)
{
    other.data_ = nullptr;
}

Buffer::~Buffer()
{
    free(data_);
    data_ = nullptr;
}
