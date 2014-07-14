#pragma once

#include <cstdint>
#include <ctime>
#include <memory>
#include <array>
#include <functional>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "Packet.h"
#include "core/Range.h"


// error callback
typedef std::function<void(const boost::system::error_code)> ErrorCallback;

// read callback
typedef std::function<void(int64_t, ByteRange)>     ReadCallback;


// transferred data stats
struct TransferStats
{
    uint32_t    total_send_count = 0;
    uint32_t    total_send_size = 0;
    uint32_t    total_recv_count = 0;
    uint32_t    total_recv_size = 0;
};


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
    uint64_t    GetLastRecvTime() const { return last_recv_time_; }

private:
    // handle completion of a read operation.
    void HandleReadHead(const boost::system::error_code& err, size_t bytes);

    void HandleReadContent(const boost::system::error_code& err,
                           size_t bytes, 
                           uint8_t* buf);

    // handle completion of a write operation.
    void HandleWrite(const boost::system::error_code& err, 
                     size_t bytes, 
                     uint8_t* buf);

    bool CheckHeader(boost::system::error_code& err, size_t bytes);
    bool CheckContent(boost::system::error_code& err, const uint8_t* buf, size_t bytes);

private:
    // socket for the connection.
    boost::asio::ip::tcp::socket        socket_;

    bool            stopped_ = false;

    // recv header
    Header          head_;
    StackBuffer     stack_buf_;

    // serial number of this connection
    int64_t         serial_ = 0;

    uint64_t        last_recv_time_ = 0;

    // error callback
    ErrorCallback    on_error_;

    // read data callback
    ReadCallback     on_read_;

    TransferStats    stats_;
};

typedef std::shared_ptr<TcpConnection>  TcpConnectionPtr;
