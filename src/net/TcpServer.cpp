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
      acceptor_(io_service),
      drop_dead_timer_(io_service, boost::posix_time::seconds(1)),
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

    // drop dead connections in every second
    drop_dead_timer_.async_wait(std::bind(&TcpServer::DropDeadConnections, this));
}

void TcpServer::Stop()
{
    acceptor_.cancel();
    connections_.clear();
}

void TcpServer::Close(Serial serial)
{
    connections_.erase(serial);
}

TcpConnectionPtr  TcpServer::GetConnection(Serial serial) const
{
    auto iter = connections_.find(serial);
    if (iter != connections_.end())
    {
        return iter->second;
    }
    return TcpConnectionPtr();
}

void TcpServer::SendTo(Serial serial, const void* data, uint32_t size)
{
    auto conn = GetConnection(serial);
    if (conn)
    {
        conn->AsynWrite(data, size);
    }
}

void TcpServer::SendAll(const char* data, uint32_t size)
{
    for (auto& value : connections_)
    {
        auto& connection = value.second;
        connection->AsynWrite(data, size);
    }
}


void TcpServer::StartAccept()
{
    while (connections_.count(current_serial_++))
        ;
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(io_service_, current_serial_, on_read_);
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


void TcpServer::DropDeadConnections()
{
    std::vector<Serial> dead_connections;
    dead_connections.reserve(32);
    time_t now = time(NULL);
    for (auto& item : connections_)
    {
        TcpConnectionPtr& conn = item.second;
        if (conn->GetClosed())
        {
            dead_connections.emplace_back(item.first);
            continue;
        }

        auto elapsed = now - conn->GetLastRecvTime();
        if (elapsed >= options_.heart_beat_sec * 1000000000UL)
        {
            conn->Close();
            dead_connections.emplace_back(item.first);
        }
        else
        {
            conn->UpdateTransferFreqency(1);
            const TransferStats& stats = conn->GetTransferStats();
            if (stats.peak_recv_num_per_sec > options_.max_recv_num_per_sec
                || stats.peak_recv_size_per_sec > options_.max_recv_size_per_sec)
            {
                conn->Close();
                dead_connections.emplace_back(item.first);
            }
        }
    }

    for (auto serial : dead_connections)
    {
        Close(serial);
    }

    drop_dead_timer_.expires_from_now(boost::posix_time::seconds(1));
    drop_dead_timer_.async_wait(std::bind(&TcpServer::DropDeadConnections, this));
}

const TransferStats* TcpServer::GetConnectionStats(Serial serial) const
{
    auto conn = GetConnection(serial);
    if (conn)
    {
        return &conn->GetTransferStats();
    }
    return nullptr;
}

std::unordered_map<Serial, TransferStats>  TcpServer::GetTotalStats() const
{
    std::unordered_map<Serial, TransferStats> result;
    for (auto& value : connections_)
    {
        auto& connection = value.second;
        auto stats_ptr = &connection->GetTransferStats();
        if (stats_ptr)
        {
            result[value.first] = *stats_ptr;
        }
    }
    return std::move(result);
}
