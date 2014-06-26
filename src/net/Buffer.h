#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>

enum
{
    MAX_BODY_LEN = 1024 * 1024,     // 1M
};

struct Header
{
    uint32_t     size;              // 长度
    uint32_t     size_crc;          // 长度CRC
    uint32_t     body_crc;          // 内容CRC
};


class Buffer : boost::noncopyable
{
public:
    typedef uint8_t     value_type;
    typedef uint8_t*    pointer_type;
    typedef uint8_t&    reference_type;

    explicit Buffer(size_t size);
    Buffer(const void* data, size_t size);
    Buffer(Buffer&& other);
    ~Buffer();

    size_t              size() const { return size_; }
    pointer_type        data() { return data_; }
    const pointer_type  data() const { return data_; }
    
    pointer_type         begin() { return data(); }
    const pointer_type   begin() const { return data(); }
    pointer_type         end() { return data() + size(); }
    const pointer_type  end() const { return data() + size(); }

private:
    const size_t        size_;
    pointer_type        data_;
};


typedef std::shared_ptr<Buffer>     BufferPtr;
