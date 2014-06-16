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

void TCPServer::AsynSend(int64_t serial, const char* data, size_t size)
{
    auto iter = connections_.find(serial);
    if (iter != connections_.end())
    {
        auto& conn = iter->second;
        conn->AsynSend(data, size);
    }
    else
    {
        // log
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
    TCPConnectionPtr conn = std::make_shared<TCPConnection>(io_service_, serial);
    acceptor_.async_accept(conn->GetSocket(), std::bind(&TCPServer::HandleAccept, this, _1, conn));
}


void TCPServer::HandleAccept(const boost::system::error_code& err, TCPConnectionPtr conn)
{
    if (!err)
    {
        connections_[conn->GetSerial()] = conn;
        conn->AsynRead();
    }
    else
    {
        conn.reset();
    }
    if (acceptor_.is_open())
    {
        StartAccept();
    }
}
