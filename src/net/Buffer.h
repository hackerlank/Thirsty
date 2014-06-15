#pragma once

#include <cstddef>
#include <memory>
#include <boost/noncopyable.hpp>

class Buffer : boost::noncopyable
{
public:
    explicit Buffer(size_t size);
    Buffer(const void* data, size_t size);
    Buffer(Buffer&& other);
    ~Buffer();

    size_t      size() const { return size_; }
    char*       data() { return data_; }
    const char* data() const { return data_; }

    char*       begin() { return data(); }
    const char* begin() const { return data(); }
    char*       end() { return begin() + size(); }
    const char* end() const { return begin() + size(); }

private:
    const size_t    size_;
    char*           data_;
};

typedef std::shared_ptr<Buffer>     BufferPtr;
