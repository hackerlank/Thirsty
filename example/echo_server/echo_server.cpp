#include <ctime>
#include <iostream>
#include <string>
#include "net/TCPServer.h"
#include "core/Conv.h"


using namespace std;


int main(int argc, char* argv[])
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
    echo_server->Start(host, port, [&](int64_t serial, ByteRange range)
    {
        time_t now = time(NULL);
        const char* date = ctime(&now);
        printf("recv %d bytes from serial %d at %s.\n", range.size(), serial, date);
        echo_server->SendTo(serial, range);
    });
    printf("server started at %s:%s.\n", argv[1], argv[2]);

    io_service.run();

    return 0;
}
