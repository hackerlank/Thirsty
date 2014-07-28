#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>
#include "core/Range.h"
#include "net/Packet.h"
#include "Timer.h"


class TcpClient : private boost::noncopyable
{
public:
    typedef std::function<void(const std::string&, int16_t)>    ConnectCallback;
    typedef std::function<void(ByteRange)>                      ReadCallback;

public:
    explicit TcpClient(boost::asio::io_service& io_service);
    ~TcpClient();

    // post an asyn connec
    void    AsynConnect(const std::string& host, int16_t port, ConnectCallback callback);

    // start read data from server
    void    StartRead(ReadCallback callback);

    // close this connection
    void    Close(const boost::system::error_code& ec = boost::system::error_code());
    
    // send data to server
    bool    AsynWrite(const void* data, uint32_t bytes);

    bool    AsynWrite(ByteRange range) { return AsynWrite(range.data(), (uint32_t)(range.size())); }

    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

    // is this connection closed
    bool GetClosed() const { return closed_; }

    const boost::system::error_code GetLastError() const { return last_err_; }

private:
    // post a read request
    void    AsynReadHead();

    // send a heartbeat packet
    void    AsynWriteHeartbeat();

    // handle connect event
    void    HandleConnect(const boost::system::error_code& ec, 
                          const std::string& host, int16_t port);

    // handle write event
    void    HandleWrite(const boost::system::error_code& ec, size_t bytes, uint8_t* buf);

    // read head handler
    void    HandleReadHead(const boost::system::error_code& err, size_t bytes);

    // read body handler
    void    HandleReadContent(const boost::system::error_code& err, size_t bytes);
                           
    bool    CheckContent(const uint8_t* buf, size_t bytes);

private:
    // tcp socket object
    boost::asio::ip::tcp::socket    socket_;

    // packet header
    Header                  head_;

    // recv packet buffer
    std::vector<uint8_t>    recv_buf_;

    // is this connection closed
    bool    closed_ = false;

    // last error code
    boost::system::error_code last_err_;

    // connect callback handler
    ConnectCallback     on_connect_;

    // read data callback handler
    ReadCallback        on_read_;
};

typedef std::shared_ptr<TcpClient>  TcpClientPtr;
