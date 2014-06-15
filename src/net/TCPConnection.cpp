#include "TCPConnection.h"
#include <functional>
#include <zlib.h>
#include "core/logging.h"

using namespace std::placeholders;


TCPConnection::TCPConnection(boost::asio::io_service& io_service, int64_t serial)
    : socket_(io_service),
      serial_(serial)
{
}

TCPConnection::~TCPConnection()
{
}

void TCPConnection::Close()
{
    socket_.close();
}

void TCPConnection::Start()
{
    boost::asio::async_read(socket_, boost::asio::buffer(buf_->data(), buf_->head_size()),
        std::bind(&TCPConnection::HandleReadHeader, this, _1, _2));
}

void TCPConnection::HandleReadHeader(const boost::system::error_code& err, size_t bytes)
{
    if(!err)
    {
        const Header& head = buf_->header();
        if (bytes == sizeof(head) && head.size <= MAX_BODY_LEN)
        {
            if (buf_->check_head_crc())
            {
                buf_->resize(head.size);
                boost::asio::async_read(socket_, boost::asio::buffer(buf_->body(), head.size),
                    std::bind(&TCPConnection::HandleReadBody, this, _1, _2));
            }
            else
            {
                LOG(ERROR) << "invalid head checksum, " << serial_ << ", size: " << head.size
                    << ", checksum: " << head.size_crc;
            }
        }
        else
        {
            LOG(ERROR) << "header size invalid, serial: " << serial_ << ", size: " << head.size;
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
        const Header& head = buf_->header();
        if (bytes == buf_->body_size())
        {            
            if (buf_->check_body_crc())
            {

            }
            else
            {
                LOG(ERROR) << "invalid body checksum, " << serial_ << ", size: " << head.size
                    << ", checksum: " << head.body_crc;
            }
        }
        else
        {
            LOG(ERROR) << "body size invalid, serial: " << serial_ << ", size: " << head.size;
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

