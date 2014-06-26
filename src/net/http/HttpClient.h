#pragma once

#include <memory>
#include <boost/noncopyable.hpp>

class HttpClient : boost::noncopyable
{
public:
    HttpClient();
    ~HttpClient();
private:
};

typedef std::shared_ptr<HttpClient>     HttpClientPtr;
