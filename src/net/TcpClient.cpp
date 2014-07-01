#include "TcpClient.h"
#include <functional>
#include <zlib.h>
#include "core/logging.h"
#include "core/Strings.h"

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
    head.size_crc = crc32(0, (const Bytef*)&head.size, sizeof(head.size));
    head.body_crc = crc32(0, (const Bytef*)data, bytes);
    BufferPtr buf = std::make_shared<Buffer>(bytes + sizeof(head));
    memcpy(buf->data(), &head, sizeof(head));
    memcpy(buf->data() + sizeof(head), data, bytes);
    boost::asio::async_write(socket_, boost::asio::buffer(buf->data(), buf->size()),
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
                            BufferPtr buf)
{
    if (err)
    {
        on_error_(err.value(), err.message());
    }
}

void TcpClient::HandleReadHead(const boost::system::error_code& err, size_t bytes)
{
    if (err)
    {
        on_error_(err.value(), err.message());
        return;
    }
    if (bytes == sizeof(head_) && head_.size <= MAX_BODY_LEN)
    {
        auto checksum = crc32(0, (const Bytef*)&head_.size, sizeof(head_.size));
        if (checksum == head_.size_crc)
        {
            BufferPtr buf = std::make_shared<Buffer>(head_.size);
            boost::asio::async_read(socket_, boost::asio::buffer(buf->data(), buf->size()),
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

void TcpClient::HandleReadBody(const boost::system::error_code& err,
                               size_t bytes,
                               BufferPtr buf)
{
    if (err)
    {
        on_error_(err.value(), err.message());
        return;
    }

    if (bytes == head_.size)
    {
        auto checksum = crc32(0, (const Bytef*)buf->data(), buf->size());
        if (checksum == head_.body_crc)
        {
            on_read_(ByteRange(buf->data(), buf->size()));
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
