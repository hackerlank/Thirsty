#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "core/Range.h"
#include "Header.h"


typedef std::function<void(int32_t, const std::string&)>    ErrorCallback;
typedef std::function<void(const std::string&, int16_t)>    ConnectCallback;
typedef std::function<void(ByteRange)>                      ReadCallback;


class TcpClient
    : public std::enable_shared_from_this<TcpClient>,
      private boost::noncopyable
{
public:
    TcpClient(boost::asio::io_service& io_service, ErrorCallback callback);
    ~TcpClient();

    void    AsynRead(ReadCallback callback);
    void    Close();

    void    AsynConnect(const std::string& host, 
                        int16_t port, 
                        ConnectCallback callback);
    
    // send data to server
    void    AsynWrite(const void* data, size_t bytes);
    void    AsynWrite(ByteRange range) { AsynWrite(range.data(), range.size()); }

    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

private:

    // post a read request
    void    AsynReadHead();

    // handle connect event
    void    HandleConnect(const boost::system::error_code& err, 
                          const std::string& host, 
                          int16_t port);

    // handle write event
    void    HandleWrite(const boost::system::error_code& err, 
                        size_t bytes, 
                        uint8_t* buf);

    // read head handler
    void    HandleReadHead(const boost::system::error_code& err, size_t bytes);

    // read body handler
    void    HandleReadBody(const boost::system::error_code& err, 
                           size_t bytes,
                           uint8_t* buf);
private:
    boost::asio::io_service&        io_service_;
    boost::asio::ip::tcp::socket    socket_;

    Header              head_;   // message header
    StackBuffer         stack_buf_; // stacked buffer

    ConnectCallback     on_connect_;
    ReadCallback        on_read_;
    ErrorCallback       on_error_;
};

typedef std::shared_ptr<TcpClient>  TcpClientPtr;
