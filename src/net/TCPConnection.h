#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "Buffer.h"

typedef std::function<void (int64_t, int, const std::string&)>    ErrorHandler;

class TCPConnection
    : public std::enable_shared_from_this<TCPConnection>,
      private boost::noncopyable
{
public:
    // Construct a connection with the given io_service.
    TCPConnection(boost::asio::io_service& io_service, int64_t serial, ErrorHandler error_callback);
    ~TCPConnection();

    // Start the first asynchronous operation for the connection.
    void AsynRead();

    // Send messages
    void AsynSend(const char* data, size_t size);

    // Stop this connection
    void Close();

    // Get the socket associated with the connection.
    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

    int64_t GetSerial() const { return serial_; }

private:
    // Handle completion of a read operation.
    void HandleReadHead(const boost::system::error_code& err, size_t bytes);
    void HandleReadBody(const boost::system::error_code& err, size_t bytes);

    // Handle completion of a write operation.
    void HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr ptr);

private:
    // Socket for the connection.
    boost::asio::ip::tcp::socket        socket_;

    // recv buffer
    Buffer          recv_buf_;

    // serial number of this connection
    int64_t         serial_ = 0;

    // error callback
    ErrorHandler    on_error_;
};

typedef std::shared_ptr<TCPConnection>  TCPConnectionPtr;
