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
    enum {RESERVED_SIZE = 20};

    Buffer();
    Buffer(const void* data, size_t size);
    Buffer(Buffer&& other);
    ~Buffer();
    
    size_t          size() const { return data_.size(); }
    char*           data() { return &data_[0]; }
    const char*     data() const { return &data_[0]; }    
    Header*         header() { return reinterpret_cast<Header*>(data()); }
    const Header*   header() const { return reinterpret_cast<const Header*>(data()); }

    size_t          body_size() const { return header()->size; }
    char*           body() { return data() + sizeof(Header); }
    const char*     body() const { return data() + sizeof(Header); }

    void            reserve_body(size_t bodysize);

    bool            check_head_crc() const;
    bool            check_body_crc() const;
    
    void            make_head_checksum();
    void            make_body_checksum();
    
private:
    std::vector<char>       data_;
};


typedef std::shared_ptr<Buffer>     BufferPtr;
