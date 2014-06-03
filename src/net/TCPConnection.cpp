#include "TCPConnection.h"
#include <functional>
#include "TCPServer.h"
#include "Buffer.h"


TCPConnection::TCPConnection(boost::asio::io_service& io_service, int serial)
    : socket_(io_service),
      strand_(io_service),
      serial_(serial))
{
}

boost::asio::io_service::socket& TCPConnection::GetSocket()
{
    return socket_;
}

int TCPConnection::GetSerial()
{
    return serial_;
}

void TCPConnection::Stop()
{
    socket_.close();
}

void TCPConnection::StartRead()
{
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)), 
        strand_.wrap(std::bind(&TCPConnection::HandleReadHeader, shared_from_this(), _1, _2)));
}

void TCPConnection::HandleReadHeader(const boost::system::error_code& err, size_t bytes)
{
    if(!err)
    {
        if (bytes == sizeof(head_) && head_.body_size <= Header::MAX_BODY_SIZE)
        {
            int32_t checksum = crcChecksum(&head_.body_size, sizeof(head_.body_size));
            if (checksum == head_.size_checksum)
            {
                BufferPtr bp(new Buffer(head_.body_size));
                async_read(socket_, buffer(data_.body(), data_size), strand_.wrap(
                    std::bind(&TCPConnection::HandleReadBody, shared_from_this(), _1, _2)));
                return ;
            }
        }
    }
}

void TCPConnection::HandleReadBody(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        int data_size = data_.body_size();
        boost::crc_32_type crc32;
        crc32.process_bytes(data_.body(), data_size);
        if (data_.checksum() == crc32.checksum())
        {
            DataPtr data_ptr(new char[data_size]);
            memcpy(data_ptr.get(), data_.body(), data_size);
            Command accept_cmd = { kCmdRecv, serial_, 0, data_size, data_ptr };
            this_server_.PushCommand(accept_cmd);
            return;
        }
    }
}


void TCPConnection::AsynSend(const char* data, size_t size)
{
}

void TCPConnection::HandleWrite(const boost::system::error_code& err)
{
}

} // namespace arkto
