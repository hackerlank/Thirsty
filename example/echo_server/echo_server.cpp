#include <ctime>
#include <iostream>
#include <string>
#include "Platform.h"
#include "net/TcpServer.h"
#include "core/Conv.h"
#include "Timer.h"

using namespace std;
using namespace std::placeholders;


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
        cout << "Serial: " << item.first << endl;
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

        boost::asio::io_service io_service;
        TcpServerPtr server = StartServer(io_service, host, port);

        // create timer
        TimerPtr timer = make_shared<Timer>(io_service, 5000,
            std::bind(PrintConnectionStats, _1, server));
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
