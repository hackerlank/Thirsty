#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "TCPConnection.h"
#include "Timer.h"

enum
{
    kHeartBeatCheckTime = 180,      // check every 3 minutes
    kConnectionDeadTime = 600,      // no more than 5 minutes
};


class TCPServer : private boost::noncopyable
{
public:
    explicit TCPServer(boost::asio::io_service& io_service, size_t max_conn);
    ~TCPServer();

    void Start(const std::string& addr, 
               const std::string& port,
               ReadCallback callback);

    void Stop();

    // close a connection
    void CloseSession(int64_t serial);

    // send data to connection
    void SendTo(int64_t serial, const char* data, size_t size);

    void SendAll(const char* data, size_t size);

    TCPConnectionPtr  GetConnection(int64_t serial);

private:
    void StartAccept();
    void HandleAccept(const boost::system::error_code& err, TCPConnectionPtr ptr);
    void OnConnectionError(int64_t serial, int error, const std::string& msg);
    void DropDeadConnections();

private:
    // the io_service used to perform asynchronous operations.
    boost::asio::io_service&        io_service_;

    // acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor  acceptor_;

    // connections identified by serial number
    std::unordered_map<int64_t, TCPConnectionPtr>    connections_;

    // current serial number
    int64_t         current_serial_ = 1000;

    // read data callback
    ReadCallback    on_read_;

    // max connection limit
    const size_t    max_connections_ = 2000;

    // heartbeat checking
    TimerPtr        heartbeat_timer_;

};

typedef std::shared_ptr<TCPServer>    TCPServerPtr;
