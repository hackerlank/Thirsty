#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "core/Range.h"
#include "Packet.h"
#include "Timer.h"


typedef std::function<void(const boost::system::error_code&)>   ErrorCallback;
typedef std::function<void(const std::string&, int16_t)>        ConnectCallback;
typedef std::function<void(ByteRange)>                          ReadCallback;


class TcpClient
    : public std::enable_shared_from_this<TcpClient>,
      private boost::noncopyable
{
public:
    TcpClient(boost::asio::io_service& io_service, 
              uint32_t heartbeat_sec, 
              ErrorCallback callback);
    ~TcpClient();

    void    AsynRead(ReadCallback callback);
    void    Close();

    void    AsynConnect(const std::string& host, int16_t port, ConnectCallback callback);
    
    // send data to server
    void    AsynWrite(const void* data, uint32_t bytes);
    void    AsynWrite(ByteRange range) 
    { 
        AsynWrite(range.data(), static_cast<uint32_t>(range.size())); 
    }

    boost::asio::ip::tcp::socket&   GetSocket() { return socket_; }

private:
    // post a read request
    void    AsynReadHead();

    // send a heartbeat packet
    void    AsynWriteHeartbeat();

    // handle connect event
    void    HandleConnect(const boost::system::error_code& err, 
                          const std::string& host, int16_t port);
                          

    // handle write event
    void    HandleWrite(const boost::system::error_code& err, 
                        size_t bytes, uint8_t* buf);
                        

    // read head handler
    void    HandleReadHead(const boost::system::error_code& err, size_t bytes);

    // read body handler
    void    HandleReadContent(const boost::system::error_code& err, 
                              size_t bytes, uint8_t* buf);
                           
    bool CheckHeader(boost::system::error_code& err, size_t bytes);
    bool CheckContent(boost::system::error_code& err, const uint8_t* buf, size_t bytes);

private:
    boost::asio::io_service&        io_service_;
    boost::asio::ip::tcp::socket    socket_;

    Header              head_;      // message header
    StackBuffer         stack_buf_; // small packet buffer

    ConnectCallback     on_connect_;
    ReadCallback        on_read_;
    ErrorCallback       on_error_;

    uint32_t            heartbeat_sec_ = 60;
    TimerPtr            heartbeat_timer_;
};

typedef std::shared_ptr<TcpClient>  TcpClientPtr;
