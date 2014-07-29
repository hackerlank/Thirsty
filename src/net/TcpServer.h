#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include "net/TcpConnection.h"
#include "core/Range.h"


class TcpServer : private boost::noncopyable
{
public:
    explicit TcpServer(boost::asio::io_service& io_service, const ServerOptions& options);
    ~TcpServer();

    void Start(const std::string& addr,
               int16_t port,
               ReadCallback callback);

    void Stop();

    // close a connection
    void Close(Serial serial);

    // send data to connection
    void SendTo(Serial serial, const void* data, uint32_t size);

    void SendTo(Serial serial, ByteRange range)
    {
        SendTo(serial, range.data(), static_cast<uint32_t>(range.size()));
    }

    // send data to all connections
    void SendAll(const char* data, uint32_t size);

    // get a conncetion by serial number
    TcpConnectionPtr  GetConnection(Serial serial) const;

    // get connection transfer statics by serial number
    const TransferStats* GetConnectionStats(Serial serial) const;

    // get all connection's transfer statics
    std::unordered_map<Serial, TransferStats>  GetTotalStats() const;

private:
    void StartAccept();
    void HandleAccept(const boost::system::error_code& err, TcpConnectionPtr ptr);
    void DropDeadConnections();

private:
    // the io_service used to perform asynchronous operations.
    boost::asio::io_service&        io_service_;

    // acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor  acceptor_;

    // connections identified by serial number
    std::unordered_map<Serial, TcpConnectionPtr>    connections_;

    // current serial number
    Serial  current_serial_ = 1000;

    // read data callback
    ReadCallback    on_read_;

    // server options
    ServerOptions   options_;

    // dead_line timer for dropping dead connections
    boost::asio::steady_timer    drop_dead_timer_;
};

typedef std::shared_ptr<TcpServer>    TcpServerPtr;
