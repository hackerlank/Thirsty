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
        timer->Schedule();
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
    TimerPtr timer = make_shared<Timer>(io_service, 100, std::bind(TimeoutHandle, 
        _1, std::ref(io_service), &counter));
    timer->Schedule();
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
        TimerPtr timer = make_shared<Timer>(io_service, 0, std::bind(TimeoutHandle2, 
            _1, std::ref(io_service), &gcounter));
        timer->Schedule();
        vec.emplace_back(timer);
    }
    size_t r = io_service.run();
    EXPECT_TRUE(r == gcounter * times);
}
