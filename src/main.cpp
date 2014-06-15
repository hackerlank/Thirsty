#include <iostream>
#include "net/TCPServer.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << "Usage: Thirsty [host] [port].\n";
        return 1;
    }

    boost::asio::io_service io_service;
    TCPServer server(io_service);
    server.Start(argv[1], argv[2]);
    cout << "server start at " << argv[1] << ": " << argv[2] << endl;
    io_service.run();

    return 0;
}
