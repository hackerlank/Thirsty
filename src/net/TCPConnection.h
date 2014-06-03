#pragma once

#include <memory>
#include <boost/noncopyable.hpp>
#include "Header.h"

class TCPServer;

class TCPConnection 
    : public std::enable_shared_from_this<TCPConnection>,
      private boost::noncopyable
{
public:
    /// Construct a connection with the given io_service.
    TCPConnection(boost::asio::io_service& io_service, int serial);

    /// Start the first asynchronous operation for the connection.
    void AsynRead(char* data, size_t size);

    /// Send messages
    void AsynSend(const char* data, size_t size);

    /// Stop this connection
    void Stop();

    /// Get the socket associated with the connection.
    boost::asio::ip::tcp::socket&   GetSocket();

    int GetSerial();


private:
    /// Handle completion of a read operation.
    void HandleReadHeader(const boost::system::error_code& e, std::size_t bytes_transferred);

    void HandleReadBody(const boost::system::error_code& e, std::size_t bytes_transferred);
        
    /// Handle completion of a write operation.
    void HandleWrite(const boost::system::error_code& e);
    
private:
    /// Socket for the connection.
    boost::asio::ip::tcp::socket        socket_;

    /// Strand to ensure the connection's handlers are not called concurrently.
    boost::asio::io_service::strand     strand_;

    int         serial_;

    Header      head_;
};

typedef std::shared_ptr<TCPConnection>  TCPConnectionPtr;

