#pragma once

#include <cstddef>
#include <cstdint>
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

bool CheckHeadCrc(const Header& head);
bool CheckBodyCrc(const Header& head, const void* data, size_t len);


class Buffer : boost::noncopyable
{
public:
    enum kResetType
    {
        RESET_HEAD = 0x01,
        RESET_BODY = 0x10,
    };

    Buffer();
    Buffer(const void* data, size_t size);
    Buffer(Buffer&& other);
    ~Buffer();

    size_t          size() const { return sizeof(head_) + head_.size; }
    char*           data() { return reinterpret_cast<char*>(&head_); }
    const char*     data() const { return reinterpret_cast<const char*>(&head_); }

    size_t          head_size() const { return sizeof(head_); }
    const Header&   header() const { return *reinterpret_cast<const Header*>(&head_); }

    size_t          body_size() const { return head_.size; }
    char*           body() { return body_; };
    const char*     body() const { return body_; }

    void            resize(size_t size);
    void            resize(const void* data, size_t size);

    bool            check_head_crc() const;
    bool            check_body_crc() const;
    
    void            make_head_checksum();
    void            make_body_checksum();
private:
    Header      head_;
    char*       body_;
};

typedef std::shared_ptr<Buffer>     BufferPtr;
