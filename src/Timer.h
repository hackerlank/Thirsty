#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

class Timer;
typedef std::shared_ptr<Timer>      TimerPtr;


class Timer : private boost::noncopyable
              
{
public:
    typedef std::function<void()>   CallbackType;

    explicit Timer(boost::asio::io_service& io_service, uint32_t expire, CallbackType callback);
    ~Timer();

    void    Schedule();
    void    Cancel();

private:
    void    HandleTimeout(const boost::system::error_code& err);
    
    bool    canceled_ = false;

    boost::asio::deadline_timer     timer_;

    uint32_t        expire_ = ~0;
    CallbackType    callback_ = nullptr;
};


