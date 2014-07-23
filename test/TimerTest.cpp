#include "Timer.h"
#include <memory>
#include <functional>
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include "Benchmark.h"


using namespace std;
using namespace std::placeholders;


static void TimeoutHandle(TimerPtr timer, 
                          boost::asio::io_service& io_service, 
                          size_t* counter)
{
    size_t& count = *counter;
    if (--count > 0)
    {
        printf("Testing timer, schedule count: %d\n", count);
        timer->Schedule(timer->GetExpire(), std::bind(TimeoutHandle, timer,
            std::ref(io_service), counter));
    }
    else
    {
        io_service.stop();
    }
}

TEST(Timer, Schedule)
{
    boost::asio::io_service io_service;
    static size_t counter = 10;
    int gcount = counter;
    TimerPtr timer = make_shared<Timer>(io_service);
    timer->Schedule(100, std::bind(TimeoutHandle, timer, 
        std::ref(io_service), &counter));
    auto r = io_service.run();
    EXPECT_TRUE(r == gcount);
}

static void TimeoutHandle2(TimerPtr timer, 
                           boost::asio::io_service& io_service, 
                           int* counter)
{
    if (--*counter = 0)
    {
        io_service.stop();
    }
}

BENCHMARK(TimerSchedule, iter)
{
    boost::asio::io_service io_service;
    int gcounter = 1000000;
    int times = gcounter;
    vector<TimerPtr> vec;
    for (int i = 0; i < times; i++)
    {
        TimerPtr timer = make_shared<Timer>(io_service);
        timer->Schedule(0, std::bind(TimeoutHandle2, timer, std::ref(io_service), &gcounter));
        vec.emplace_back(timer);
    }
    size_t r = io_service.run();
    EXPECT_TRUE(r == gcounter * times);
}
