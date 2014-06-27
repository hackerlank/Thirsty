#include "HttpClient.h"
#include <cassert>
#include <functional>
#include "core/logging.h"

using namespace std::placeholders;
using namespace boost::asio;

static http_parser_settings req_parser_settings;

//////////////////////////////////////////////////////////////////////////
HttpClient::HttpClient(boost::asio::io_service& io_service, 
                       const std::string& node, 
                       const std::string& service)
    : io_service_(io_service),
      resolver_(io_service),
      socket_(io_service),
      query_(node, service)
{
}

HttpClient::~HttpClient()
{
    socket_.close();
}

void  HttpClient::SendRequest(const std::string& s, ReadCallback callback)
{
    assert(callback);
    read_callback_ = callback;
    request_ = &s;
    resolver_.async_resolve(query_, std::bind(&HttpClient::HandleResolve, this, _1, _2));
}


void  HttpClient::HandleResolve(const boost::system::error_code& err, 
                                ip::tcp::resolver::iterator iterator)
{
    if (!err)
    {
        http_parser_init(&parser_, HTTP_RESPONSE);
        parser_.data = this;
        async_connect(socket_, iterator, std::bind(&HttpClient::HandleConnect, this, _1));
    }
    else
    {
        LOG(ERROR) << err.value() << ": " << err.message();
    }
}
    
void  HttpClient::HandleConnect(const boost::system::error_code& err)
{
    if (!err)
    {
        async_write(socket_, buffer(request_->c_str(), request_->size()),
            std::bind(&HttpClient::HandleWrite, this, _1, _2));

        socket_.async_read_some(buffer(response_.data(), response_.size()),
            std::bind(&HttpClient::HandleRead, this, _1, _2));
    }
}

void  HttpClient::HandleWrite(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
    }
    else
    {
        LOG(ERROR) << err.value() << ": " << err.message();
    }
}

void  HttpClient::HandleRead(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        auto parsed = http_parser_execute(&parser_, &req_parser_settings,
            response_.data(), response_.size());
        if (parser_.upgrade)
        {
        }
        else if (parsed != bytes)
        {
            LOG(ERROR) << http_errno_description(HTTP_PARSER_ERRNO(&parser_));
        }
    }
    else
    {
        LOG(ERROR) << err.value() << ": " << err.message();
    }
}

//////////////////////////////////////////////////////////////////////////
//
// http parsing
//


static int onMessageBegin(http_parser* parser)
{
    assert(parser);
    HttpClient* http_client = reinterpret_cast<HttpClient*>(parser->data);
    assert(http_client);
    return 0;
}

static int onUrl(http_parser* parser, const char* at, size_t length)
{
    assert(parser && at);
    HttpClient* http_client = reinterpret_cast<HttpClient*>(parser->data);
    assert(http_client);
    http_client->SetUri(StringPiece(at, length));
    return 0;
}

static int onHeaderField(http_parser* parser, const char* at, size_t length)
{
    assert(parser && at);
    HttpClient* http_client = reinterpret_cast<HttpClient*>(parser->data);
    assert(http_client);
    http_client->AddHeaderField(std::string(at, length));
    return 0;
}

static int onHeaderValue(http_parser* parser, const char* at, size_t length)
{
    assert(parser && at);
    HttpClient* http_client = reinterpret_cast<HttpClient*>(parser->data);
    assert(http_client);
    http_client->AddHeaderValue(std::string(at, length));
    
    return 0;
}

static int onBody(http_parser* parser, const char* at, size_t length)
{
    assert(parser && at);
    HttpClient* http_client = reinterpret_cast<HttpClient*>(parser->data);
    assert(http_client);
    http_client->InvokeCallback(StringPiece(at, length));
    return 0;
}

static int onHeadersComplete(http_parser* parser)
{
    assert(parser);
    HttpClient* http_client = reinterpret_cast<HttpClient*>(parser->data);
    assert(http_client);
    return 0;
}

static int onMessageComplete(http_parser* parser)
{
    assert(parser);
    HttpClient* http_client = reinterpret_cast<HttpClient*>(parser->data);
    assert(http_client);
    return 0;
}

static bool initParserSettings()
{
    req_parser_settings.on_message_begin = onMessageBegin;
    req_parser_settings.on_url = onUrl;
    req_parser_settings.on_header_field = onHeaderField;
    req_parser_settings.on_header_value = onHeaderValue;
    req_parser_settings.on_headers_complete = onHeadersComplete;
    req_parser_settings.on_body = onBody;
    req_parser_settings.on_message_complete = onMessageComplete;
    return true;
}

static bool init_parser_setting = initParserSettings();
