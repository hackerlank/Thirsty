#include "TCPConnection.h"
#include <functional>
#include <zlib.h>
#include "core/logging.h"
#include "core/StringPrintf.h"

using namespace std;
using namespace std::placeholders;


TCPConnection::TCPConnection(boost::asio::io_service& io_service, 
                             int64_t serial, 
                             ErrorCallback on_error,
                             ReadCallback on_read)
    : socket_(io_service),
      serial_(serial),
      on_error_(on_error),
      on_read_(on_read)
{
    assert(on_error && on_read);
}

TCPConnection::~TCPConnection()
{
    Close();
}

void TCPConnection::Close()
{
    stopped_ = true;
    socket_.close();
}

void TCPConnection::AsynRead()
{
    last_recv_time_ = time(NULL);
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
        std::bind(&TCPConnection::HandleReadHead, this, _1, _2));
}

void TCPConnection::HandleReadHead(const boost::system::error_code& err, size_t bytes)
{
    if (err)
    {
        on_error_(serial_, err.value(), err.message());
        return ;
    }
    if (bytes == sizeof(head_) && head_.size <= MAX_BODY_LEN)
    {
        auto checksum = crc32(0, (const Bytef*)&head_.size, sizeof(head_.size));
        if (checksum == head_.size_crc)
        {
            BufferPtr buf = std::make_shared<Buffer>(head_.size);
            boost::asio::async_read(socket_, boost::asio::buffer(buf->data(), buf->size()),
                std::bind(&TCPConnection::HandleReadBody, this, _1, _2, buf));
        }
        else
        {
            auto msg = stringPrintf("invalid header checksum, size: %d, checksum: %d",
                head_.size, head_.size_crc);
            on_error_(serial_, 0, msg);
        }
    }
    else
    {
        auto msg = stringPrintf("invalid header, bytes: %d, body size: %d", bytes, head_.size);
        on_error_(serial_, 0, msg);
    }
}

void TCPConnection::HandleReadBody(const boost::system::error_code& err, 
                                   size_t bytes, 
                                   BufferPtr buf)
{
    if (err)
    {
        on_error_(serial_, err.value(), err.message());
        return;
    }
    if (bytes == head_.size)
    {
        auto checksum = crc32(0, (const Bytef*)buf->data(), buf->size());
        if (checksum == head_.body_crc)
        {
            on_read_(serial_, ByteRange(buf->data(), bytes));
            AsynRead();
        }
        else
        {
            auto msg = stringPrintf("invalid body checksum, %d, expected: %d", 
                checksum, head_.body_crc);
            on_error_(serial_, 0, msg);
        }
    }
    else
    {
        auto msg = stringPrintf("invalid body size: %d, expected: %d", bytes, head_.size);
        on_error_(serial_, 0, msg);
    }
}


void TCPConnection::AsynWrite(const void* data, size_t size)
{
    assert(data && size);
    Header head = { size, 0, 0 };
    head.size_crc = crc32(0, (const Bytef*)&head.size, sizeof(head.size));
    head.body_crc = crc32(0, (const Bytef*)data, size);
    BufferPtr buf = std::make_shared<Buffer>(size + sizeof(head));
    memcpy(buf->data(), &head, sizeof(head));
    memcpy(buf->data() + sizeof(head), data, size);
    boost::asio::async_write(socket_, boost::asio::buffer(buf->data(), buf->size()),
        std::bind(&TCPConnection::HandleWrite, this, _1, _2, buf));
}

void TCPConnection::HandleWrite(const boost::system::error_code& err, 
                                size_t bytes, 
                                BufferPtr ptr)
{
    if (err)
    {
        on_error_(serial_, err.value(), err.message());
    }
}

