#include "TcpClient.h"
#include <functional>
#include "core/Strings.h"
#include "core/Malloc.h"
#include "logging.h"
#include "Checksum.h"

using namespace std;
using namespace std::placeholders;


TcpClient::TcpClient(boost::asio::io_service& io_service, ErrorCallback callback)
    : io_service_(io_service),
      socket_(io_service),
      on_error_(callback)
{
    assert(callback);
}

TcpClient::~TcpClient()
{
    Close();
}

void TcpClient::Close()
{
    socket_.close();
}

void TcpClient::AsynRead(ReadCallback callback)
{
    assert(callback);
    on_read_ = callback;
    AsynReadHead();
}

void TcpClient::AsynConnect(const std::string& host,
                            int16_t port,
                            ConnectCallback callback)
{
    assert(callback);
    on_connect_ = callback;
    using namespace boost::asio;
    ip::tcp::endpoint endpoint(ip::address::from_string(host), port);
    socket_.async_connect(endpoint, std::bind(&TcpClient::HandleConnect, this, _1, host, port));
}

void TcpClient::AsynWrite(const void* data, size_t bytes)
{
    if (data && bytes && bytes <= MAX_CONTENT_LEN)
    {
        Header head = { bytes, 0, 0 };
        size_t buf_size = bytes + sizeof(head);
        head.size_checksum = crc32c((const uint8_t*)&head.size, sizeof(head.size));
        head.content_checksum = crc32c((const uint8_t*)data, bytes);
        uint8_t* buf = (uint8_t*)checkedMalloc(goodMallocSize(buf_size));
        memcpy(buf, &head, sizeof(head));
        memcpy(buf + sizeof(head), data, bytes);
        boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
            std::bind(&TcpClient::HandleWrite, this, _1, _2, buf));
    }
}

void TcpClient::AsynReadHead()
{
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
        std::bind(&TcpClient::HandleReadHead, this, _1, _2));
}


void TcpClient::HandleConnect(const boost::system::error_code& err,
                              const std::string& host,
                              int16_t port)
{
    if (!err)
    {
        on_connect_(host, port);
    }
    else
    {
        on_error_(err);
    }
}

void TcpClient::HandleWrite(const boost::system::error_code& err,
                            size_t bytes,
                            uint8_t* buf)
{
    if (err)
    {
        on_error_(err);
    }
    free(buf);
}

void TcpClient::HandleReadHead(const boost::system::error_code& ec, size_t bytes)
{
    boost::system::error_code err = ec;
    if (!err && CheckHeader(err, bytes))
    {
        uint8_t* buf = (head_.size <= stack_buf_.size() ? stack_buf_.data()
            : (uint8_t*)checkedMalloc(goodMallocSize(head_.size)));
        boost::asio::async_read(socket_, boost::asio::buffer(buf, head_.size),
            std::bind(&TcpClient::HandleReadContent, this, _1, _2, buf));
    }
    else
    {
        on_error_(err);
    }
}

void TcpClient::HandleReadContent(const boost::system::error_code& ec, size_t bytes, uint8_t* buf)
{
    boost::system::error_code err = ec;
    if (!err && CheckContent(err, buf, bytes))
    {
        on_read_( ByteRange((const uint8_t*)buf, bytes));
        AsynReadHead();
    }
    else
    {
        on_error_(err);
    }
    if (bytes > stack_buf_.size())
    {
        free(buf);
    }
}

bool TcpClient::CheckHeader(boost::system::error_code& err, size_t bytes)
{
    err = boost::asio::error::invalid_argument;
    if (bytes == sizeof(head_))
    {
        auto checksum = crc32c((const uint8_t*)&head_.size, sizeof(head_.size));
        if (checksum == head_.size_checksum)
        {
            if (head_.size > 0 && head_.size <= MAX_CONTENT_LEN)
            {
                return true;
            }
            else
            {
                LOG(ERROR) << "invalid content size: " << head_.size;
            }
        }
        else
        {
            LOG(ERROR) << "invalid header checksum: " << checksum << ", " << head_.size_checksum;
        }
    }
    return false;
}

bool TcpClient::CheckContent(boost::system::error_code& err, const uint8_t* buf, size_t bytes)
{
    err = boost::asio::error::invalid_argument;
    if (bytes == head_.size)
    {
        auto checksum = crc32c((const uint8_t*)buf, bytes);
        if (checksum == head_.content_checksum)
        {
            return true;
        }
        else
        {
            LOG(ERROR) << "invalid content checksum: " << checksum << ", " << head_.content_checksum;
        }
    }
    return false;
}
