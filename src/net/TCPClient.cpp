#include "TCPClient.h"
#include <functional>
#include "core/logging.h"
#include "core/StringPrintf.h"

using namespace std;
using namespace std::placeholders;


TCPClient::TCPClient(boost::asio::io_service& io_service)
    : io_service_(io_service),
      socket_(io_service)
{
}

TCPClient::~TCPClient()
{
    Close();
}

void TCPClient::Close()
{
    socket_.close();
}

void TCPClient::StartRead(ReadCallback callback)
{
    assert(callback);
    on_read_ = callback;
    AsynReadHead();
}

void TCPClient::AsynConnect(const std::string& host, int16_t port, ConnectCallback callback)
{
    assert(callback);
    on_connect_ = callback;
    using namespace boost::asio;
    ip::tcp::endpoint endpoint(ip::address::from_string(host), port);
    socket_.async_connect(endpoint, std::bind(&TCPClient::HandleConnect, this, _1, host, port));
}

void TCPClient::AsynWrite(const char* data, size_t bytes)
{
    BufferPtr buf = std::make_shared<Buffer>(data, bytes);
    boost::asio::async_write(socket_, boost::asio::buffer(buf->data(), buf->size()),
        std::bind(&TCPClient::HandleWrite, this, _1, _2, buf));
}

void TCPClient::AsynReadHead()
{
    boost::asio::async_read(socket_, boost::asio::buffer(buf_.data(), sizeof(Header)),
        std::bind(&TCPClient::HandleReadHead, this, _1, _2));
}

void TCPClient::HandleConnect(const boost::system::error_code& err, 
                              const std::string& host, 
                              int16_t port)
{
    if (!err)
    {
        on_connect_(host, port);
    }
    else
    {
        LOG(ERROR) << stringPrintf("connect host[%s:%d] failed, %d:%s", host.c_str(), port,
            err.value(), err.message().c_str());
        Close();
    }
}

void TCPClient::HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr buf)
{

}

void TCPClient::HandleReadHead(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        const Header* head = buf_.header();
        if (bytes == sizeof(*head) && head->size <= MAX_BODY_LEN)
        {
            if (buf_.check_head_crc())
            {
                buf_.reserve_body(head->size);
                boost::asio::async_read(socket_, boost::asio::buffer(buf_.body(), head->size),
                    std::bind(&TCPClient::HandleReadBody, this, _1, _2));
            }
        }
    }
}

void TCPClient::HandleReadBody(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        const Header* head = buf_.header();
        if (bytes == buf_.body_size())
        {
            if (buf_.check_body_crc())
            {
                on_read_(buf_.body(), bytes);
                AsynReadHead();
            }
        }
    }
}
