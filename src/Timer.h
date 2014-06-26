#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

class Timer;
typedef std::shared_ptr<Timer>      TimerPtr;


class Timer : public std::enable_shared_from_this<Timer>,
              private boost::noncopyable
{
public:
    typedef std::function<void(TimerPtr)>   CallbackType;

    Timer(boost::asio::io_service& io_service,
          int32_t expire_time,
          CallbackType callback);

    ~Timer();

    void    Schedule();

    void    Cancel();

private:
    void    HandleTimeout(const boost::system::error_code& err);
    
    bool    canceled_ = false;

    boost::asio::deadline_timer     timer_;

    const int32_t       expire_time_ = ~0;
    CallbackType        callback_;
};


