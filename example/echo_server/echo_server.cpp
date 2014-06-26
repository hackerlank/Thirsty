#include <ctime>
#include <iostream>
#include <string>
#include "net/TCPServer.h"
#include "core/Conv.h"


using namespace std;
using namespace std::placeholders;


void OnRead(TCPServerPtr server, int64_t serial, ByteRange range)
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
        TCPServerPtr echo_server = std::make_shared<TCPServer>(io_service, 2000);
        echo_server->Start(host, port, std::bind(OnRead, echo_server, _1, _2));
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
