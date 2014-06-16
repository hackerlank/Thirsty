#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "Buffer.h"


class TCPClient
    : public std::enable_shared_from_this<TCPClient>,
      private boost::noncopyable
{
public:
    explicit TCPClient(boost::asio::io_service& io_service);
    ~TCPClient();

    void    Close();

    void    AsynConnect(const std::string& host, int16_t port);
    void    AsynWrite(const char* data, size_t bytes);
    void    AsynRead();

    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

private:
    void    HandleConnect(const boost::system::error_code& err, const std::string& host, int16_t port);
    void    HandleWrite(const boost::system::error_code& err, size_t bytes, BufferPtr buf);
    void    HandleReadHead(const boost::system::error_code& err, size_t bytes);
    void    HandleReadBody(const boost::system::error_code& err, size_t bytes);
private:
    boost::asio::io_service&        io_service_;
    boost::asio::ip::tcp::socket    socket_;

    Buffer      buf_;   // recv buffer
};
