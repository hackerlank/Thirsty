#include "TCPServer.h"
#include <functional>


using namespace std::placeholders;


TCPServer::TCPServer(boost::asio::io_service& io_service)
    : io_service_(io_service),
      acceptor_(io_service_),
      current_serial_(1000)
{
}

TCPServer::~TCPServer()
{
    Stop();
}

void TCPServer::Start(const std::string& addr, const std::string& port)
{
    using namespace boost::asio::ip;
    tcp::resolver resolver(io_service_);
    tcp::resolver::query query(addr, port);
    tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    StartAccept();
}

void TCPServer::Stop()
{
    io_service_.stop();
    connections_.clear();
}

void TCPServer::Close(int64_t serial)
{
    auto iter = connections_.find(serial);
    if (iter != connections_.end())
    {
        auto& conn = iter->second;
        conn->Close();
        connections_.erase(iter);
    }
    else
    {
        // log
    }
}

TCPConnectionPtr  TCPServer::GetConnection(int64_t serial)
{
    auto iter = connections_.find(serial);
    if (iter != connections_.end())
    {
        return iter->second;
    }
    return TCPConnectionPtr();
}

void TCPServer::AsynSend(int64_t serial, const char* data, size_t size)
{
    auto conn = GetConnection(serial);
    if (conn)
    {
        conn->AsynSend(data, size);
    }
}

void TCPServer::SendAll(const char* data, size_t size)
{
    for (auto& value : connections_)
    {
        auto& connection = value.second;
        connection->AsynSend(data, size);
    }
}


void TCPServer::StartAccept()
{
    auto serial = current_serial_++;
    TCPConnectionPtr conn = std::make_shared<TCPConnection>(io_service_, serial,
        std::bind(&TCPServer::OnConnectionError, this, _1, _2, _3));
    acceptor_.async_accept(conn->GetSocket(), std::bind(&TCPServer::HandleAccept, this, _1, conn));
}


void TCPServer::HandleAccept(const boost::system::error_code& err, TCPConnectionPtr conn)
{
    if (!err)
    {
        connections_[conn->GetSerial()] = conn;
        conn->AsynRead();
    }
    if (acceptor_.is_open())
    {
        StartAccept();
    }
}

void TCPServer::OnConnectionError(int64_t serial, int error, const std::string& msg)
{
    Close(serial);
    fprintf(stderr, "%d: %s\n", error, msg.data());
}
