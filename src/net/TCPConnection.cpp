#include "TCPConnection.h"
#include <functional>
#include <zlib.h>
#include "core/logging.h"

using namespace std::placeholders;


TCPConnection::TCPConnection(boost::asio::io_service& io_service, int64_t serial, ErrorHandler error_callback)
    : socket_(io_service),
      serial_(serial),
      on_error_(error_callback)
{
    assert(error_callback);
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
    if(!err)
    {
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
                LOG(ERROR) << "invalid head checksum, " << serial_ << ", size: " << head->size
                    << ", checksum: " << head->size_crc;
            }
        }
        else
        {
            LOG(ERROR) << "header size invalid, serial: " << serial_ << ", size: " << head->size;
        }
    }
    else
    {
        LOG(ERROR) << serial_ << ", error: " << err.value() << "," << err.message();
    }
}

void TCPConnection::HandleReadBody(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        const Header* head = recv_buf_.header();
        if (bytes == recv_buf_.body_size())
        {
            if (recv_buf_.check_body_crc())
            {
                AsynSend(recv_buf_.body(), recv_buf_.body_size());
                AsynRead();
            }
            else
            {
                LOG(ERROR) << "invalid body checksum, " << serial_ << ", size: " << head->size
                    << ", checksum: " << head->body_crc;
            }
        }
        else
        {
            LOG(ERROR) << "body size invalid, serial: " << serial_ << ", size: " << head->size;
        }
    }
    else
    {
        LOG(ERROR) << "error: " << serial_ << ", " << err.value() << "," << err.message();
    }
}


void TCPConnection::AsynSend(const char* data, size_t size)
{
    BufferPtr buf = std::make_shared<Buffer>(data, size);
    boost::asio::async_write(socket_, boost::asio::buffer(buf->data(), buf->size()),
        std::bind(&TCPConnection::HandleWrite, this, _1, _2, buf));
}

void TCPConnection::HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr ptr)
{
}

