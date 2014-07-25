#include "Timer.h"
#include <boost/date_time.hpp>
#include "logging.h"

using namespace std::placeholders;


Timer::Timer(boost::asio::io_service& io_service,
             uint32_t expire, 
             CallbackType callback)
    : timer_(io_service), expire_(expire), callback_(callback)
{
    assert(callback);
}

Timer::~Timer()
{
    if (!canceled_)
    {
        Cancel();
    }
}

void Timer::Cancel()
{
    canceled_ = true;
    timer_.cancel();
    callback_ = nullptr;
}


void Timer::Schedule()
{
    timer_.expires_from_now(boost::posix_time::milliseconds(expire_));
    timer_.async_wait(std::bind(&Timer::HandleTimeout, this, _1));
}

void Timer::HandleTimeout(const boost::system::error_code& err)
{
    if (!err)
    {
        if (callback_ && !canceled_)
        {
            callback_(shared_from_this());
        }
        else
        {
            Cancel();
        }
    }
    else
    {
        Cancel();
        LOG(ERROR) << "Timer() error, " << err.value() << ":" << err.message();
    }
}
