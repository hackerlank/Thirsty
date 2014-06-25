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
    explicit Buffer(size_t size);
    Buffer(const void* data, size_t size);
    Buffer(Buffer&& other);
    ~Buffer();

    size_t          size() const { return size_; }
    uint8_t*        data() { return &data_[0]; }
    const uint8_t*  data() const { return &data_[0]; }
    
    uint8_t*        begin() { return data(); }
    const uint8_t*  begin() const { return data(); }
    uint8_t*        end() { return data() + size(); }
    const uint8_t*  end() const { return data() + size(); }

private:
    const size_t    size_;
    uint8_t*        data_;
};


typedef std::shared_ptr<Buffer>     BufferPtr;
