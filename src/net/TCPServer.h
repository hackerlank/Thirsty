#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "TCPConnection.h"


class TCPServer : private boost::noncopyable
{
public:
    explicit TCPServer(boost::asio::io_service& io_service);
    ~TCPServer();

    void Start(const std::string& addr, 
               const std::string& port,
               ReadCallback callback);

    void Stop();

    // Close a connection
    void CloseSession(int64_t serial);

    // Send data to connection
    void SendTo(int64_t serial, const char* data, size_t size);

    void SendAll(const char* data, size_t size);

    TCPConnectionPtr  GetConnection(int64_t serial);

private:
    // Initiate an asynchronous accept operation.
    void StartAccept();
    void HandleAccept(const boost::system::error_code& err, TCPConnectionPtr ptr);

    void OnConnectionError(int64_t serial, int error, const std::string& msg);

private:
    // The io_service used to perform asynchronous operations.
    boost::asio::io_service&        io_service_;

    // Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor  acceptor_;

    // current serial number
    int64_t         current_serial_ = 1000;

    ReadCallback    on_read_;

    // Connections identified by serial number
    std::unordered_map<int64_t, TCPConnectionPtr>    connections_;

};

typedef std::shared_ptr<TCPServer>    TCPServerPtr;
