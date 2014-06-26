#include "Timer.h"
#include <cassert>
#include <boost/date_time.hpp>
#include "core/logging.h"

using namespace std::placeholders;


Timer::Timer(boost::asio::io_service& io_service,
             int32_t expire_time,
             CallbackType callback)
    : timer_(io_service),
      expire_time_(expire_time),
      callback_(callback)
{
    assert(callback);
}

Timer::~Timer()
{
    Cancel();
}

void Timer::Cancel()
{
    timer_.cancel();
    callback_ = nullptr;
}


void Timer::Schedule()
{
    timer_.expires_from_now(boost::posix_time::seconds(expire_time_));
    timer_.async_wait(std::bind(&Timer::HandleTimeout, this, _1));
}

void Timer::HandleTimeout(const boost::system::error_code& err)
{
    if (!err)
    {
        if (callback_)
        {
            callback_();
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
