#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "Buffer.h"

typedef std::function<void(const std::string&, int16_t)>    ConnectCallback;
typedef std::function<void(const char*, size_t)>            ReadCallback;

class TCPClient
    : public std::enable_shared_from_this<TCPClient>,
      private boost::noncopyable
{
public:
    TCPClient(boost::asio::io_service& io_service);
    ~TCPClient();

    void    StartRead(ReadCallback callback);
    void    Close();

    void    AsynConnect(const std::string& host, int16_t port, ConnectCallback callback);
    void    AsynWrite(const char* data, size_t bytes);

    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

private:
    void    AsynReadHead();
    void    HandleConnect(const boost::system::error_code& err, const std::string& host, int16_t port);
    void    HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr buf);
    void    HandleReadHead(const boost::system::error_code& err, size_t bytes);
    void    HandleReadBody(const boost::system::error_code& err, size_t bytes);
private:
    boost::asio::io_service&        io_service_;
    boost::asio::ip::tcp::socket    socket_;

    Buffer          buf_;   // recv buffer

    ConnectCallback     on_connect_;
    ReadCallback        on_read_;
};

typedef std::shared_ptr<TCPClient>  TCPClientPtr;
