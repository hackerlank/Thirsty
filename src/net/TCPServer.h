#pragma once

#include <string>
#include <atomic>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "TCPConnection.h"


class TCPServer : private boost::noncopyable
{
public:
    typedef std::map<int, TCPConnectionPtr>     TCPConnectionMap;

    TCPServer(boost::asio::io_service& io_service,
             const std::string& address, 
             int16_t port);

    ~TCPServer();

    /// Run this server(called in main thread)
    void Run();

    /// Close a connection
    void Close(int serial);

    /// Send data to connection
    void AsynSend(int32_t serial, const char* data, size_t size);


private:
    /// Initiate an asynchronous accept operation.
    void StartAccept();

    /// Handle completion of an asynchronous accept operation.
    void HandleAccept(int serial, const ErrorCode& e);

    /// Handle a request to stop the server.
    void HandleStop();

private:
    void ProcessCommands();
    void OnAccept(int serial, int error);
    void OnRead();
    void OnClose();

private:
    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service&        io_service_;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor  acceptor_;

    // current serial number
    std::atomic<int>     current_serial_; 

    /// The next connection to be accepted.
    TCPConnectionPtr    new_connection_;

    // Connections identified by serial number
    TCPConnectionMap    connections_;

};

typedef std::shared_ptr<TCPServer>    TCPServerPtr;

} // namespace arkto

#endif // ARKTO_SERVER_H