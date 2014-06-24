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
    socket_.close();
}

void TCPConnection::AsynRead()
{
    boost::asio::async_read(socket_, boost::asio::buffer(recv_buf_.data(), sizeof(Header)),
        std::bind(&TCPConnection::HandleReadHead, this, _1, _2));
}

void TCPConnection::HandleReadHead(const boost::system::error_code& err, size_t bytes)
{
    if (err)
    {
        on_error_(serial_, err.value(), err.message());
        return ;
    }
    const Header* head = recv_buf_.header();
    if (bytes == sizeof(*head) && head->size <= MAX_BODY_LEN)
    {
        if (recv_buf_.check_head_crc())
        {
            recv_buf_.reserve_body(head->size);
            boost::asio::async_read(socket_, boost::asio::buffer(recv_buf_.body(), head->size),
                std::bind(&TCPConnection::HandleReadBody, this, _1, _2));
            return ;
        }
        else
        {
            LOG(ERROR) << stringPrintf("%s[%d]: invalid checksum or body size, %d, %d", __FUNCTION__,
                serial_, head->size, head->size_crc);
        }
    }
    else
    {
        LOG(ERROR) << "header size invalid, serial: " << serial_ << ", size: " << head->size;
    }
}

void TCPConnection::HandleReadBody(const boost::system::error_code& err, size_t bytes)
{
    if (err)
    {
        on_error_(serial_, err.value(), err.message());
        return;
    }
    const Header* head = recv_buf_.header();
    if (bytes == recv_buf_.body_size())
    {
        if (recv_buf_.check_body_crc())
        {
            on_read_(serial_, recv_buf_.body(), bytes);
            AsynRead();
        }
        else
        {
            LOG(ERROR) << stringPrintf("%s[%d]: invalid body checksum,", __FUNCTION__, serial_, 
                head->size_crc, head->body_crc);
        }
    }
    else
    {
        LOG(ERROR) << "body size invalid, serial: " << serial_ << ", size: " << head->size;
    }
}


void TCPConnection::AsynWrite(const char* data, size_t size)
{
    BufferPtr buf = std::make_shared<Buffer>(data, size);
    boost::asio::async_write(socket_, boost::asio::buffer(buf->data(), buf->size()),
        std::bind(&TCPConnection::HandleWrite, this, _1, _2, buf));
}

void TCPConnection::HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr ptr)
{
}

