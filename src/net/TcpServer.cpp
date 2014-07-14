#include "TcpServer.h"
#include <functional>
#include <vector>
#include <boost/date_time.hpp>
#include "core/Strings.h"
#include "Utils.h"
#include "logging.h"


using namespace std::placeholders;


TcpServer::TcpServer(boost::asio::io_service& io_service, const ServerOptions& options)
    : io_service_(io_service),
      acceptor_(io_service_),
      options_(options)
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
        std::bind(&TcpServer::HandleError, this, _1, serial), on_read_);
    acceptor_.async_accept(conn->GetSocket(), std::bind(&TcpServer::HandleAccept, this, _1, conn));
}


void TcpServer::HandleAccept(const boost::system::error_code& err, TcpConnectionPtr conn)
{
    if (!err)
    {
        if (connections_.size() < options_.max_connections)
        {
            connections_[conn->GetSerial()] = conn;
            conn->AsynRead();
        }
        else
        {
            LOG(ERROR) << "max connection count limit: " << options_.max_connections;
        }
    }
    if (acceptor_.is_open())
    {
        StartAccept();
    }
}

void TcpServer::HandleError(const boost::system::error_code& ec, int64_t serial)
{
    CloseSession(serial);
    LOG(ERROR) << serial << ", " << ec.value() << ": " << ec.message();
}

void TcpServer::DropDeadConnections()
{
    std::vector<int64_t> dead_connections;
    dead_connections.reserve(32);
    uint64_t now = getNowTickCount();
    for (auto& item : connections_)
    {
        auto& conn = item.second;
        auto elapsed = now - conn->GetLastRecvTime();
        if (elapsed >= options_.heart_beat_sec * 1000000000UL)
        {
            dead_connections.emplace_back(item.first);
        }
    }

    boost::system::error_code ec;
    for (auto serial : dead_connections)
    {
        HandleError(ec, serial);
    }
}
