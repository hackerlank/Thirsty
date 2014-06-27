#pragma once

#include <cstdint>
#include <memory>
#include <ctime>
#include <functional>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "core/Range.h"
#include "Buffer.h"


typedef std::function<void(int64_t, int32_t, const std::string&)>   ErrorCallback;
typedef std::function<void(int64_t, ByteRange)>                     ReadCallback;


class TcpConnection
    : public std::enable_shared_from_this<TcpConnection>,
      private boost::noncopyable
{
public:
    // construct a connection with the given io_service.
    TcpConnection(boost::asio::io_service& io_service, 
                  int64_t serial, 
                  ErrorCallback on_error,
                  ReadCallback on_read);
    ~TcpConnection();

    // start the first asynchronous operation for the connection.
    void AsynRead();

    // Send messages
    void AsynWrite(const void* data, size_t size);

    // stop this connection
    void Close();

    // get the socket associated with the connection.
    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

    int64_t     GetSerial() const { return serial_; }
    time_t      GetLastRecvTime() const { return last_recv_time_; }

private:
    // handle completion of a read operation.
    void HandleReadHead(const boost::system::error_code& err, size_t bytes);
    void HandleReadBody(const boost::system::error_code& err, size_t bytes, BufferPtr ptr);

    // handle completion of a write operation.
    void HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr ptr);

private:
    // socket for the connection.
    boost::asio::ip::tcp::socket        socket_;

    bool            stopped_ = false;

    // recv header
    Header          head_;

    // serial number of this connection
    int64_t         serial_ = 0;

    time_t          last_recv_time_ = 0;

    // error callback
    ErrorCallback    on_error_;

    // read data callback
    ReadCallback     on_read_;
};

typedef std::shared_ptr<TcpConnection>  TCPConnectionPtr;
