#pragma once

#include <cstdint>
#include <ctime>
#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "net/Packet.h"
#include "core/Range.h"



class TcpConnection : private boost::noncopyable
{
public:
    // construct a connection with the given io_service.
    TcpConnection(boost::asio::io_service& io_service, 
                  Serial serial,
                  ReadCallback on_read);
    ~TcpConnection();

    // start the first asynchronous operation for the connection.
    void    AsynRead();

    // send messages
    bool    AsynWrite(const void* data, uint32_t size);

    // stop this connection
    void    Close();

    // is this connection closed
    bool    GetClosed() { return closed_; }

    // get socket descriptor
    boost::asio::ip::tcp::socket& GetSocket() { return socket_; }

    // serial number of this conncetion
    Serial  GetSerial() const { return serial_; }

    // last recv data timestamp
    time_t  GetLastRecvTime() const { return last_recv_time_; }

    // conncetion transfer statics
    const TransferStats& GetTransferStats() const { return stats_; }

    void UpdateTransferFreqency(int32_t sec);

private:
    // handle completion of a read operation.
    void HandleReadHead(const boost::system::error_code& err, size_t bytes);

    void HandleReadContent(const boost::system::error_code& err, size_t bytes);

    // handle completion of a write operation.
    void HandleWrite(const boost::system::error_code& err, 
                     size_t bytes, 
                     uint8_t* buf);

    bool CheckContent(const uint8_t* buf, size_t bytes);

    void UpdateTransferStats(size_t bytes_read, size_t bytes_send);    

private:
    // socket for the connection.
    boost::asio::ip::tcp::socket        socket_;

    // is this connectin closed
    bool            closed_ = false;

    // serial number of this connection
    Serial          serial_ = 0;

    // timestamp of last recv data
    time_t          last_recv_time_ = 0;

    // packet header
    Header          head_;

    std::vector<uint8_t> recv_buf_;

    // read data callback
    ReadCallback    on_read_;

    // transfer statics
    TransferStats   stats_;

    // recv packet count dure last 
    uint32_t        last_recv_count_ = 0;
    float           last_recv_bytes_ = 0;
};

typedef std::shared_ptr<TcpConnection>  TcpConnectionPtr;
