#include "TcpServer.h"
#include <functional>
#include <vector>
#include <boost/date_time.hpp>
#include "core/logging.h"
#include "core/Strings.h"


using namespace std::placeholders;


TcpServer::TcpServer(boost::asio::io_service& io_service, size_t max_conn)
    : io_service_(io_service),
      acceptor_(io_service_),
      max_connections_(max_conn)
{
}

TcpServer::~TcpServer()
{
    Stop();
}

void TcpServer::Start(const std::string& addr,
                      int16_t port,
                      ReadCallback callback)
{
    using namespace boost::asio::ip;
    assert(callback);
    on_read_ = callback;
    tcp::endpoint endpoint(address::from_string(addr), port);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    StartAccept();

    if (!heartbeat_timer_)
    {
        heartbeat_timer_ = std::make_shared<Timer>(io_service_, kHeartBeatCheckTime,
            std::bind(&TcpServer::DropDeadConnections, this));
    }
    heartbeat_timer_->Schedule();
}

void TcpServer::Stop()
{
    acceptor_.cancel();
    connections_.clear();
}

void TcpServer::CloseSession(int64_t serial)
{
    connections_.erase(serial);
}

TcpConnectionPtr  TcpServer::GetConnection(int64_t serial)
{
    auto iter = connections_.find(serial);
    if (iter != connections_.end())
    {
        return iter->second;
    }
    return TcpConnectionPtr();
}

void TcpServer::SendTo(int64_t serial, const void* data, size_t size)
{
    auto conn = GetConnection(serial);
    if (conn)
    {
        conn->AsynWrite(data, size);
    }
}

void TcpServer::SendAll(const char* data, size_t size)
{
    for (auto& value : connections_)
    {
        auto& connection = value.second;
        connection->AsynWrite(data, size);
    }
}


void TcpServer::StartAccept()
{
    auto serial = current_serial_++;
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(io_service_, serial,
        std::bind(&TcpServer::OnConnectionError, this, _1, _2, _3), on_read_);
    acceptor_.async_accept(conn->GetSocket(), std::bind(&TcpServer::HandleAccept, this, _1, conn));
}


void TcpServer::HandleAccept(const boost::system::error_code& err, TcpConnectionPtr conn)
{
    if (!err)
    {
        if (connections_.size() < max_connections_)
        {
            connections_[conn->GetSerial()] = conn;
            conn->AsynRead();
        }
        else
        {
            LOG(ERROR) << "max connection limit: " << max_connections_;
        }
    }
    if (acceptor_.is_open())
    {
        StartAccept();
    }
}

void TcpServer::OnConnectionError(int64_t serial, int error, const std::string& msg)
{
    CloseSession(serial);
    LOG(ERROR) << "serial " << serial << " closed, " << error << ": " << msg;
}

void TcpServer::DropDeadConnections()
{
    std::vector<int64_t> dead_connections;
    dead_connections.reserve(32);
    time_t now = time(NULL);
    for (auto& item : connections_)
    {
        auto& conn = item.second;
        auto elapsed = now - conn->GetLastRecvTime();
        if (elapsed >= kConnectionDeadTime)
        {
            dead_connections.emplace_back(item.first);
        }
    }
    for (auto serial : dead_connections)
    {
        CloseSession(serial);
    }

    heartbeat_timer_->Schedule();
}
