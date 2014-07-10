#include "net/http/HttpClient.h"
#include <gtest/gtest.h>

using namespace std;

TEST(HttpClient, SendRequest)
{
    boost::asio::io_service io_service;

    HttpClient http_client(io_service, "www.qq.com", "http");
    std::string request =
        "GET /index.html HTTP/1.1\r\n"
        "Host: 0.0.0.0=8000\r\n"
        "User-Agent: Chrome\r\n"
        "Keep-Alive: 100\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";

    http_client.SendRequest(request, [&](StringPiece s)
    {
        cout << string(s.data(), s.size()) << endl;
    });

    auto r = io_service.run();
    EXPECT_EQ(r, 1);
}