#include "TcpClient.h"
#include <functional>
#include "core/Strings.h"
#include "core/Malloc.h"
#include "logging.h"
#include "Checksum.h"

using namespace std;
using namespace std::placeholders;


TcpClient::TcpClient(boost::asio::io_service& io_service, ErrorCallback callback)
    : io_service_(io_service),
      socket_(io_service),
      on_error_(callback)
{
    assert(callback);
}

TcpClient::~TcpClient()
{
    Close();
}

void TcpClient::Close()
{
    socket_.close();
}

void TcpClient::AsynRead(ReadCallback callback)
{
    assert(callback);
    on_read_ = callback;
    AsynReadHead();
}

void TcpClient::AsynConnect(const std::string& host,
                            int16_t port,
                            ConnectCallback callback)
{
    assert(callback);
    on_connect_ = callback;
    using namespace boost::asio;
    ip::tcp::endpoint endpoint(ip::address::from_string(host), port);
    socket_.async_connect(endpoint, std::bind(&TcpClient::HandleConnect, this, _1, host, port));
}

void TcpClient::AsynWrite(const void* data, size_t bytes)
{
    assert(data && bytes);
    Header head = { bytes, 0, 0 };
    size_t buf_size = bytes + sizeof(head);
    head.size_crc = crc32c((const uint8_t*)&head.size, sizeof(head.size));
    head.body_crc = crc32c((const uint8_t*)data, bytes);
    uint8_t* buf = (uint8_t*)checkedMalloc(goodMallocSize(buf_size));
    memcpy(buf, &head, sizeof(head));
    memcpy(buf + sizeof(head), data, bytes);
    boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
        std::bind(&TcpClient::HandleWrite, this, _1, _2, buf));
}

void TcpClient::AsynReadHead()
{
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
        std::bind(&TcpClient::HandleReadHead, this, _1, _2));
}


void TcpClient::HandleConnect(const boost::system::error_code& err,
                              const std::string& host,
                              int16_t port)
{
    if (!err)
    {
        on_connect_(host, port);
    }
    else
    {
        on_error_(err.value(), err.message());
    }
}

void TcpClient::HandleWrite(const boost::system::error_code& err,
                            size_t bytes,
                            uint8_t* buf)
{
    if (err)
    {
        on_error_(err.value(), err.message());
    }
    free(buf);
}

void TcpClient::HandleReadHead(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        if (bytes == sizeof(head_) && head_.size <= MAX_BODY_LEN)
        {
            auto body_size = head_.size;
            auto checksum = crc32c((const uint8_t*)&body_size, sizeof(body_size));
            if (checksum == head_.size_crc)
            {
                uint8_t* buf = (body_size <= stack_buf_.size() ? stack_buf_.data()
                    : (uint8_t*)checkedMalloc(goodMallocSize(body_size)));
                boost::asio::async_read(socket_, boost::asio::buffer(buf, body_size),
                    std::bind(&TcpClient::HandleReadBody, this, _1, _2, buf));
            }
            else
            {
                auto msg = stringPrintf("invalid header checksum: %d, expected: %d", checksum, head_.body_crc);
                on_error_(0, msg);
            }
        }
        else
        {
            auto msg = stringPrintf("invalid header size, bytes: %d, body: %d", bytes, head_.size);
            on_error_(0, msg);
        }
    }
    else
    {
        on_error_(err.value(), err.message());
    }
}

void TcpClient::HandleReadBody(const boost::system::error_code& err,
                               size_t bytes,
                               uint8_t* buf)
{
    if (err)
    {
        if (bytes == head_.size)
        {
            auto checksum = crc32c((const uint8_t*)buf, bytes);
            if (checksum == head_.body_crc)
            {
                on_read_(ByteRange(buf, bytes));
                AsynReadHead();
            }
            else
            {
                auto msg = stringPrintf("invalid body checksum: %d, expected: %d", checksum, head_.body_crc);
                on_error_(0, msg);
            }
        }
        else
        {
            auto msg = stringPrintf("invalid body size: %d, expected: %d", bytes, head_.size);
            on_error_(0, msg);
        }
    }
    else
    {
        on_error_(err.value(), err.message());
    }

    if (bytes > stack_buf_.size())
    {
        free(buf);
    }
}
