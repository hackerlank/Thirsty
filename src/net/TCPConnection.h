#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "Buffer.h"


class TCPConnection 
    : public std::enable_shared_from_this<TCPConnection>,
      private boost::noncopyable
{
public:
    // Construct a connection with the given io_service.
    TCPConnection(boost::asio::io_service& io_service, int64_t serial);
    ~TCPConnection();

    // Start the first asynchronous operation for the connection.
    void Start();

    // Send messages
    void AsynSend(const char* data, size_t size);

    // Stop this connection
    void Close();

    // Get the socket associated with the connection.
    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

    int64_t GetSerial() const { return serial_; }

private:
    // Handle completion of a read operation.
    void HandleReadHeader(const boost::system::error_code& err, size_t bytes);
    void HandleReadBody(const boost::system::error_code& err, size_t bytes);
        
    // Handle completion of a write operation.
    void HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr ptr);
    
private:
    // Socket for the connection.
    boost::asio::ip::tcp::socket        socket_;

    int64_t     serial_;
    BufferPtr   buf_;
};

typedef std::shared_ptr<TCPConnection>  TCPConnectionPtr;
