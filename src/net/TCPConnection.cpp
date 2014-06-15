#include "TCPConnection.h"
#include <functional>
#include <zlib.h>
#include "core/logging.h"


TCPConnection::TCPConnection(boost::asio::io_service& io_service, int64_t serial)
    : socket_(io_service),
      serial_(serial)
{
}

void TCPConnection::Close()
{
    socket_.close();
}

void TCPConnection::Start()
{
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)), 
        [this](const boost::system::error_code& err, size_t bytes)
    {
        this->HandleReadHeader(err, bytes);
    });
}

void TCPConnection::HandleReadHeader(const boost::system::error_code& err, size_t bytes)
{
    if(!err)
    {
        if (bytes == sizeof(head_) && head_.size <= MAX_BODY_LEN)
        {
            auto checksum = crc32(0, (const Bytef*)&head_.size, sizeof(head_.size));
            if (checksum == head_.size_checksum)
            {
                BufferPtr bufptr(new Buffer(head_.size));
                boost::asio::async_read(socket_, boost::asio::buffer(bufptr->data(), bufptr->size()),
                    [&](const boost::system::error_code& err, size_t  bytes)
                {
                    this->HandleReadBody(err, bytes, bufptr);
                });
            }
        }
    }
}

void TCPConnection::HandleReadBody(const boost::system::error_code& err, size_t bytes, BufferPtr ptr)
{
    if (!err)
    {
        if (bytes == ptr->size())
        {
            auto checksum = crc32(0, (const Bytef*)ptr->data(), ptr->size());
            if (head_.body_checksum == checksum)
            {

            }
        }
    }
}


void TCPConnection::AsynSend(const char* data, size_t size)
{
    BufferPtr ptr(new Buffer(data, size));
    boost::asio::async_write(socket_, boost::asio::buffer(ptr->data(), ptr->size()),
        [&](const boost::system::error_code& err, size_t bytes)
    {
        HandleWrite(err, bytes, ptr);
    });
}

void TCPConnection::HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr ptr)
{
}

