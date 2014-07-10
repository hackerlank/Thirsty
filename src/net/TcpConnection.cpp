#include "TcpConnection.h"
#include <functional>
#include <zlib.h>
#include "core/Malloc.h"
#include "core/Strings.h"
#include "logging.h"


using namespace std;
using namespace std::placeholders;


TcpConnection::TcpConnection(boost::asio::io_service& io_service,
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

TcpConnection::~TcpConnection()
{
    Close();
}

void TcpConnection::Close()
{
    stopped_ = true;
    socket_.close();
}

void TcpConnection::AsynRead()
{
    last_recv_time_ = time(NULL);
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
        std::bind(&TcpConnection::HandleReadHead, this, _1, _2));
}

void TcpConnection::HandleReadHead(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        if (bytes == sizeof(head_) && head_.size <= MAX_BODY_LEN)
        {
            auto body_size = head_.size;
            auto checksum = crc32(0, (const Bytef*)&body_size, sizeof(body_size));
            if (checksum == head_.size_crc && body_size > 0)
            {
                void* buf = (body_size <= stack_buf_.size() ? stack_buf_.data() 
                    : checkedMalloc(goodMallocSize(body_size)));
                boost::asio::async_read(socket_, boost::asio::buffer(buf, head_.size),
                    std::bind(&TcpConnection::HandleReadBody, this, _1, _2, buf));
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
    else
    {
        on_error_(serial_, err.value(), err.message());
    }
}

void TcpConnection::HandleReadBody(const boost::system::error_code& err,
                                   size_t bytes,
                                   void* buf)
{
    if (!err)
    {
        if (bytes == head_.size)
        {
            auto checksum = crc32(0, (const Bytef*)buf, bytes);
            if (checksum == head_.body_crc)
            {
                on_read_(serial_, ByteRange((const uint8_t*)buf, bytes));
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
    else
    {
        on_error_(serial_, err.value(), err.message());
    }
    if (bytes > stack_buf_.size())
    {
        free(buf);
    }
}


void TcpConnection::AsynWrite(const void* data, size_t size)
{
    // to-do
    // support sending size more than MAX_BODY_LEN 
    assert(data && size && size <= MAX_BODY_LEN);
    Header head = { size, 0, 0 };
    head.size_crc = crc32(0, (const Bytef*)&head.size, sizeof(head.size));
    head.body_crc = crc32(0, (const Bytef*)data, size);
    size_t buf_size = size + sizeof(head);
    void* buf = checkedMalloc(goodMallocSize(buf_size));
    memcpy((char*)buf, &head, sizeof(head));
    memcpy((char*)buf + sizeof(head), data, size);
    boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
        std::bind(&TcpConnection::HandleWrite, this, _1, _2, buf));
}

void TcpConnection::HandleWrite(const boost::system::error_code& err,
                                size_t bytes,
                                void* buf)
{
    if (err)
    {
        on_error_(serial_, err.value(), err.message());
    }
    free(buf);
}

