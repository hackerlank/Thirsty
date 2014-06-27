#pragma once


#include <string>
#include <array>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "http_parser.h"
#include "URI.h"


class HttpClient : boost::noncopyable
{
public:
    typedef std::function<void(StringPiece)>      ReadCallback;

    HttpClient(boost::asio::io_service& io_service, 
               const std::string& node, 
               const std::string& service);
    ~HttpClient();

    void  SendRequest(const std::string& s, ReadCallback callback);
    void  SetUri(StringPiece s) { uri_ = std::make_shared<Uri>(s); }
    void  AddHeaderField(const std::string& s) { header_fields_.emplace_back(s); }
    void  AddHeaderValue(const std::string& s) { header_values_.emplace_back(s); }

    void  InvokeCallback(StringPiece s) { if (read_callback_){ read_callback_(s); } }

private:
    void  HandleResolve(const boost::system::error_code& err, 
                        boost::asio::ip::tcp::resolver::iterator iter);
    void  HandleConnect(const boost::system::error_code& err);
    void  HandleWrite(const boost::system::error_code& err, size_t bytes);
    void  HandleRead(const boost::system::error_code& err, size_t bytes);
private:

    boost::asio::io_service&                io_service_;
    boost::asio::ip::tcp::socket            socket_;
    boost::asio::ip::tcp::resolver::query   query_;
    boost::asio::ip::tcp::resolver          resolver_;
    
    const std::string*      request_ = nullptr;
    std::array<char, 8192>  response_;

    std::shared_ptr<Uri>   uri_;

    std::vector<std::string>    header_fields_;
    std::vector<std::string>    header_values_;

    ReadCallback    read_callback_;

    http_parser     parser_;
};

typedef std::shared_ptr<HttpClient>     HttpClientPtr;
