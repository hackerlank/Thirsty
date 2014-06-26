#include "Timer.h"
#include <memory>
#include <functional>
#include <gtest/gtest.h>
#include <boost/asio.hpp>

using namespace std;
using namespace std::placeholders;
using boost::asio::io_service;

typedef shared_ptr<io_service>  io_servie_ptr;

static void TimeoutHandle(TimerPtr timer, io_servie_ptr ios_ptr, size_t* counter)
{
    size_t& count = *counter;
    if (--count > 0)
    {
        printf("Testing timer, schedule count: %d\n", count);
        timer->Schedule();
    }
    else
    {
        ios_ptr->stop();
    }
}

TEST(Timer, Schedule)
{
    io_servie_ptr ios_ptr = make_shared<io_service>();
    static size_t counter = 10;
    int gcount = counter;
    TimerPtr timer = make_shared<Timer>(*ios_ptr, 100, std::bind(TimeoutHandle, _1, ios_ptr, &counter));
    timer->Schedule();
    auto r = ios_ptr->run();
    EXPECT_TRUE(r == gcount);
}

// how many timers to bench
#ifdef NDEBUG
size_t global_counter = 100000;
#else
size_t global_counter = 1000;
#endif

static void TimeoutHandle2(TimerPtr timer, int* counter, io_servie_ptr ios_ptr)
{
    if (--*counter > 0)
    {
        timer->Schedule();
    }
    else
    {
        delete counter;
        timer->Cancel();
        if (--global_counter == 0)
        {
            ios_ptr->stop();
        }
    }
}

TEST(Timer, Benchmark)
{
    io_servie_ptr ios_ptr = make_shared<io_service>();
    int gcounter = global_counter;
    int times = 5;
    vector<TimerPtr> vec;
    for (int i = 0; i < global_counter; i++)
    {
        int* counter = new int(times);
        int expire = rand() % 300 + 50;
        auto timer = make_shared<Timer>(*ios_ptr, expire, std::bind(TimeoutHandle2, _1, 
            counter, ios_ptr));
        timer->Schedule();
        vec.emplace_back(timer);
    }
    printf("Benchmarking %d timer with %d times each....\n", gcounter, times);
    size_t r = ios_ptr->run();
    EXPECT_TRUE(r == gcounter * times);
}
