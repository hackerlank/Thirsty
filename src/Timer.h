#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>


class Timer : private boost::noncopyable
{
public:
    typedef std::function<void()>   CallbackType;

    Timer(boost::asio::io_service& io_service,
          int32_t expire_time,
          CallbackType callback);

    ~Timer();

    void Again() { Schedule(); }

private:
    void    Schedule();
    void    HandleTimeout(const boost::system::error_code& err);
    void    Cancel();

    boost::asio::deadline_timer     timer_;

    const int32_t       expire_time_ = 0;   // milliseconds
    CallbackType        callback_;
};

typedef std::shared_ptr<Timer>      TimerPtr;
