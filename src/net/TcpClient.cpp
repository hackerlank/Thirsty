#include "TcpClient.h"
#include <boost/asio.hpp>
#include <functional>
#include "core/Strings.h"
#include "core/Malloc.h"
#include "logging.h"
#include "Checksum.h"
#include "Utils.h"

using namespace std;
using namespace std::placeholders;


TcpClient::TcpClient(boost::asio::io_service& io_service)
    : socket_(io_service)
{
}

TcpClient::~TcpClient()
{
    Close();
}

void TcpClient::Close(const boost::system::error_code& ec)
{
    if (!closed_)
    {
        last_err_ = ec;
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
        socket_.close();
        closed_ = true;
    }
}

void TcpClient::StartRead(ReadCallback callback)
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

bool TcpClient::AsynWrite(const void* data, uint32_t bytes)
{
    if (data == nullptr || bytes == 0 || bytes > MAX_CONTENT_LEN)
    {
        return false;
    }

    Header head = { bytes, 0, 0 };
    size_t buf_size = bytes + sizeof(head);
    head.checksum = crc32c(data, bytes);
    uint8_t* buf = (uint8_t*)checkedMalloc(goodMallocSize(buf_size));
    memcpy(buf, &head, sizeof(head));
    memcpy(buf + sizeof(head), data, bytes);
    boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
        std::bind(&TcpClient::HandleWrite, this, _1, _2, buf));
    return true;
}

void TcpClient::AsynWriteHeartbeat()
{
    Header head = { 0, 0, 0 };
    boost::asio::async_write(socket_, boost::asio::buffer(&head, sizeof(head)),
        std::bind(&TcpClient::HandleWrite, this, _1, _2, nullptr));
}

void TcpClient::AsynReadHead()
{
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
        std::bind(&TcpClient::HandleReadHead, this, _1, _2));
}


void TcpClient::HandleConnect(const boost::system::error_code& ec,
                              const std::string& host,
                              int16_t port)
{
    if (!ec)
    {
        on_connect_(host, port);
    }
    else
    {
        Close(ec);
    }
}

void TcpClient::HandleWrite(const boost::system::error_code& ec,
                            size_t bytes,
                            uint8_t* buf)
{
    free(buf);
    if (ec)
    {
        Close(ec);
    }
}

void TcpClient::HandleReadHead(const boost::system::error_code& ec, size_t bytes)
{
    if (!ec)
    {
        if (head_.size <= MAX_CONTENT_LEN)
        {
            if (recv_buf_.size() < head_.size)
            {
                recv_buf_.resize(head_.size);
            }
            boost::asio::async_read(socket_, boost::asio::buffer(recv_buf_.data(), 
                head_.size), std::bind(&TcpClient::HandleReadContent, this, _1, _2));
        }
        else
        {
            Close();
        }
    }
    else
    {
        Close();
    }
}

void TcpClient::HandleReadContent(const boost::system::error_code& ec, size_t bytes)
{
    if (!ec)
    {
        const uint8_t* data = recv_buf_.data();
        if (CheckContent(data, bytes))
        {
            on_read_(ByteRange(data, bytes));
            AsynReadHead();
        }
        else
        {
            Close();
        }
    }
    else
    {
        Close(ec);
    }
}

bool TcpClient::CheckContent(const uint8_t* buf, size_t bytes)
{
    if (buf && bytes > 0 && bytes == head_.size)
    {
        auto checksum = crc32c(buf, bytes);
        if (checksum == head_.checksum)
        {
            return true;
        }
        else
        {
            LOG(ERROR) << "invalid content checksum: " << checksum << ", " << head_.checksum;
        }
    }
    return false;
}
