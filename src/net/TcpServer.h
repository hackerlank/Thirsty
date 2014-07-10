#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "TcpConnection.h"
#include "Timer.h"
#include "core/Range.h"

enum
{
    kHeartBeatCheckTime = 180,      // check every 3 minutes
    kConnectionDeadTime = 600,      // no more than 5 minutes
};

struct ServerOptions
{
    uint16_t  heart_beat_sec = 100;         // 心跳时间
    uint16_t  max_connections = 5000;       // 最大连接数
    uint16_t  max_send_size_per_sec = 1024; // 每秒最大发送
    uint16_t  max_recv_size_per_sec = 1024; // 每秒最大接收
};

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
    void CloseSession(int64_t serial);

    // send data to connection
    void SendTo(int64_t serial, const void* data, size_t size);

    void SendTo(int64_t serial, ByteRange range)
    {
        SendTo(serial, range.data(), range.size());
    }

    void SendAll(const char* data, size_t size);

    TcpConnectionPtr  GetConnection(int64_t serial);

private:
    void StartAccept();
    void HandleAccept(const boost::system::error_code& err, TcpConnectionPtr ptr);
    void OnConnectionError(int64_t serial, int error, const std::string& msg);
    void DropDeadConnections();

private:
    // the io_service used to perform asynchronous operations.
    boost::asio::io_service&        io_service_;

    // acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor  acceptor_;

    // connections identified by serial number
    std::unordered_map<int64_t, TcpConnectionPtr>    connections_;

    // current serial number
    int64_t         current_serial_ = 1000;

    // read data callback
    ReadCallback    on_read_;

    // server options
    ServerOptions   options_;

    // heartbeat checking
    TimerPtr        heartbeat_timer_;

};

typedef std::shared_ptr<TcpServer>    TcpServerPtr;
