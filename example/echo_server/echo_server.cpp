#include <ctime>
#include <iostream>
#include <string>
#include "Platform.h"
#include "net/TcpServer.h"
#include "core/Conv.h"
#include "Timer.h"
#include "Utils.h"

using namespace std;
using namespace std::placeholders;


void MyLogHandler(LogLevel level,
                  const char* filename,
                  int line,
                  const string& message)
{
    static const char* level_names[] = { "INFO", "WARNING", "ERROR", "FATAL" };
    LogFileM(level_names[level], "[%s:%d] %s\n", filename, line, message.c_str());
}

TcpServerPtr StartServer(boost::asio::io_service& io_service, const string& host, uint16_t port)
{
    ServerOptions opt;
    TcpServerPtr echo_server = make_shared<TcpServer>(io_service, opt);
    echo_server->Start(host, port, [echo_server](Serial serial, ByteRange data)
    {
        echo_server->SendTo(serial, data);
    });
    printf("server started at %s:%d.\n", host.c_str(), port);
    return echo_server;
}


void PrintConnectionStats(TimerPtr timer, TcpServerPtr server)
{
    auto stats = server->GetTotalStats();
    for (auto& item : stats)
    {
        const auto& stats = item.second;
        cout << "Serial: " << item.first << endl
            << "\ttotal_send_count: " << stats.total_send_count << endl
            << "\ttotal_recv_count: " << stats.total_recv_count << endl
            << "\ttotal_send_size: " << stats.total_send_size << endl
            << "\total_recv_size: " << stats.total_recv_size << endl
            << "\tpeak_recv_num_per_sec: " << stats.peak_recv_num_per_sec << endl
            << "\tpeak_recv_size_per_sec: " << stats.peak_recv_size_per_sec << endl
            << endl;
    }
    timer->Schedule();
}


int main(int argc, char* argv[])
{
    try
    {
        string host = "127.0.0.1";
        int16_t port = 32450;
        if (argc >= 3)
        {
            host = argv[1];
            port = to<int16_t>(argv[2]);
        }

        SetLogHandler(MyLogHandler);

        boost::asio::io_service io_service;
        TcpServerPtr server = StartServer(io_service, host, port);

        // create timer
        TimerPtr timer;
        auto callback = [&]()
        {
            PrintConnectionStats(timer, server);
        };
        timer.reset(new Timer(io_service, 10000, callback));
        timer->Schedule();

        io_service.run();
    }
    catch (const std::exception& ex)
    {
        cout << typeid(ex).name() << ": " << ex.what() << endl;
        return 1;
    }


    return 0;
}
