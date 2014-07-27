#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "Platform.h"
#include "net/TcpClient.h"
#include "core/Conv.h"
#include "core/Range.h"
#include "Timer.h"

using namespace std;


void DropDeadClients(TimerPtr timer, vector<TcpClientPtr>& clients)
{
    for (auto& client : clients)
    {
        if (client && client->GetClosed())
        {
            client.swap(TcpClientPtr());
        }
    }
    timer->Schedule();
}

TcpClientPtr CreateClient(boost::asio::io_service& io_service,
                          const std::string& host,
                          int16_t port)
{
    TcpClientPtr client = make_shared<TcpClient>(io_service);
    client->AsynConnect(host, port, [client](const string& host, int16_t port)
    {
        printf("connect to server(%s:%d) OK.\n", host.c_str(), port);
        string msg = "a quick fox jumps over the lazy dog.";
        client->AsynWrite(msg.data(), msg.length()+1);
        client->StartRead([client](ByteRange range)
        {
            client->AsynWrite(range);
        });
    });
    return client;
}

int main(int argc, char* argv[])
{
    try
    {
        string host = "127.0.0.1";
        int16_t port = 32450;
        int32_t num = 1;
        if (argc >= 4)
        {
            host = argv[1];
            port = to<int16_t>(argv[2]);
            num = to<int32_t>(argv[3]);
        }

        vector<TcpClientPtr>    clients;
        boost::asio::io_service io_service;
        for (int i = 0; i < num; i++)
        {
            auto client = CreateClient(io_service, host, port);
            clients.emplace_back(client);
        }

        // create timer
        TimerPtr timer;
        auto callback = [&]()
        {
            DropDeadClients(timer, clients);
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
