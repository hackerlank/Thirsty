#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "net/TCPClient.h"
#include "core/Conv.h"
#include "core/Range.h"

using namespace std;


void OnError(int32_t error, const string& msg)
{
    printf("Error: %d, %s.\n", error, msg.c_str());
}

TCPClientPtr CreateClient(boost::asio::io_service& io_service, 
                          const std::string& host, 
                          int16_t port)
{
    TCPClientPtr client = std::make_shared<TCPClient>(io_service, OnError);
    client->AsynConnect(host, port, [client](const string& host, int16_t port)
    {
        printf("connect to server(%s:%d) OK.\n", host.c_str(), port);
        string msg = "a quick fox jumps over the lazy dog.";
        client->AsynWrite(msg.data(), msg.length()+1);
        client->StartRead([client](ByteRange range)
        {
            //printf("recv %d bytes from server.\n", bytes, data);
            this_thread::sleep_for(chrono::seconds(1));
            client->AsynWrite(range);
        });
    });
    return client;
}

int main(int argc, char* argv[])
{
    string host = "127.0.0.1";
    int16_t port = 32450;
    int32_t num = 2000;
    if (argc >= 4)
    {
        host = argv[1];
        port = to<int16_t>(argv[2]);
        num = to<int32_t>(argv[3]);
    }

    vector<TCPClientPtr>    clients;
    boost::asio::io_service io_service;
    for (int i = 0; i < num; i++)
    {
        auto client = CreateClient(io_service, host, port);
        clients.emplace_back(client);
    }

    io_service.run();
    return 0;
}
