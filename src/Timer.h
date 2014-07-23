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

    explicit Timer(boost::asio::io_service& io_service);
    ~Timer();

    void    Schedule(int32_t expire, CallbackType callback);
    void    Schedule(int32_t expire);

    void    Cancel();

    int32_t GetExpire() const { return expire_; }

private:
    void    HandleTimeout(const boost::system::error_code& err);
    
    bool    canceled_ = false;

    boost::asio::deadline_timer     timer_;

    int32_t         expire_ = ~0;
    CallbackType    callback_ = nullptr;
};


