#include "Buffer.h"

static char* pool_alloc(size_t size)
{
    return new char[size];
}

static void pool_free(void* data)
{
    delete[] data;
}

//////////////////////////////////////////////////////////////////////////
Buffer::Buffer(size_t size)
    : size_(size), data_(pool_alloc(size))
{
}

Buffer::Buffer(const void* data, size_t size)
    : size_(size), data_(pool_alloc(size))
{
    memcpy(data_, data, size);
}

Buffer::Buffer(Buffer&& other)
    : size_(other.size_), data_(other.data_)
{
    other.data_ = nullptr;
}

Buffer::~Buffer()
{
    pool_free(data_);
    data_ = nullptr;
}
