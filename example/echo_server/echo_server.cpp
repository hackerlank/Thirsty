#include <ctime>
#include <iostream>
#include <string>
#include "Platform.h"
#include "net/TcpServer.h"
#include "core/Conv.h"


using namespace std;
using namespace std::placeholders;


void OnRead(TcpServerPtr server, Serial serial, ByteRange range)
{
    time_t now = time(NULL);
    const char* date = ctime(&now);
    printf("recv %d bytes from serial %lld at %s.\n", range.size(), serial, date);
    server->SendTo(serial, range);
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
        ServerOptions opt;
        TcpServerPtr echo_server = make_shared<TcpServer>(io_service, opt);
        echo_server->Start(host, port, [&](Serial serial, ByteRange data)
        {
            time_t now = time(NULL);
            const char* date = ctime(&now);
            printf("recv %d bytes from serial %lld at %s", data.size(), serial, date);
            echo_server->SendTo(serial, data);
        });
        printf("server started at %s:%d.\n", host.c_str(), port);

        io_service.run();
    }
    catch (const std::exception& ex)
    {
        cout << typeid(ex).name() << ": " << ex.what() << endl;
        return 1;
    }


    return 0;
}
