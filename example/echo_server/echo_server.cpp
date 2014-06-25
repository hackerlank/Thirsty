#include <ctime>
#include <iostream>
#include <string>
#include "net/TCPServer.h"


using namespace std;

TCPServerPtr    echo_server;

void OnRead(int64_t serial, const char* data, size_t len)
{
    echo_server->SendTo(serial, data, len);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: echo_server [host] [port]");
        return 1;
    }

    boost::asio::io_service io_service;
    echo_server = std::make_shared<TCPServer>(io_service);
    echo_server->Start(argv[1], argv[2], [&](int64_t serial, const char* data, size_t bytes)
    {
        time_t now = time(NULL);
        const char* date = ctime(&now);
        printf("recv %d bytes from serial %d at %s.\n", bytes, serial, date);
        echo_server->SendTo(serial, data, bytes);
    });
    printf("server started at %s:%s.\n", argv[1], argv[2]);

    io_service.run();

    return 0;
}
