#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "net/TCPClient.h"

using namespace std;

vector<TCPClientPtr>    clients;

TCPClientPtr CreateClient(boost::asio::io_service& io_service, 
                          const std::string& host, 
                          int16_t port)
{
    TCPClientPtr client = std::make_shared<TCPClient>(io_service);
    client->AsynConnect(host, port, [client](const string& host, int16_t port)
    {
        printf("connect to server(%s:%d) OK.\n", host.c_str(), port);
        string msg = "a quick fox jumps over the lazy dog.";
        client->AsynWrite(msg.data(), msg.length()+1);
        client->StartRead([client](const char* data, size_t bytes)
        {
            //printf("recv %d bytes from server.\n", bytes, data);
            this_thread::sleep_for(chrono::seconds(1));
            client->AsynWrite(data, bytes);
        });
    });
    return client;
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("usage: echo_client [host] [port] [num]\n");
        return 1;
    }

    int16_t port = atoi(argv[2]);
    int num = atoi(argv[3]);
    boost::asio::io_service io_service;
    for (int i = 0; i < num; i++)
    {
        auto client = CreateClient(io_service, argv[1], port);
        clients.emplace_back(client);
    }
    io_service.run();
    return 0;
}
