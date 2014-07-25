#include "net/TcpServer.h"
#include <gtest/gtest.h>

using namespace std;


TEST(TCPServer, testStartServer)
{
    boost::asio::io_service io_service;
    TcpServer server(io_service, ServerOptions());
    EXPECT_EQ(0, io_service.run());
}

TEST(TCPServer, testAsynRead)
{
    boost::asio::io_service io_service;
    TcpServer server(io_service, ServerOptions());
    server.Start("127.0.0.1", 32450, [](Serial serial, ByteRange data)
    {
    });
    EXPECT_EQ(0, io_service.poll());
}

TEST(TCPServer, testAsynWrite)
{

}
