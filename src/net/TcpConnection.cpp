#include "TcpConnection.h"
#include <functional>
#include "Core/Conv.h"
#include "core/Malloc.h"
#include "core/Strings.h"
#include "logging.h"
#include "Checksum.h"
#include "Utils.h"


using namespace std;
using namespace std::placeholders;


TcpConnection::TcpConnection(boost::asio::io_service& io_service,
                             Serial serial,
                             ErrorCallback on_error,
                             ReadCallback on_read)
    : socket_(io_service),
      serial_(serial),
      on_error_(on_error),
      on_read_(on_read)
{
    assert(on_error && on_read);
    start_recv_time_ = getNowTickCount();
}

TcpConnection::~TcpConnection()
{
    Close();
}

void TcpConnection::Close()
{
    stopped_ = true;
    socket_.close();
}

void TcpConnection::AsynRead()
{
    last_recv_time_ = getNowTickCount();
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
        std::bind(&TcpConnection::HandleReadHead, this, _1, _2));
}

void TcpConnection::HandleReadHead(const boost::system::error_code& ec, size_t bytes)
{
    boost::system::error_code err = ec;
    if (!err && CheckHeader(err, bytes))
    {
        if (head_.size > 0)
        {
            uint8_t* buf = (head_.size <= stack_buf_.size() ? stack_buf_.data()
                : (uint8_t*)checkedMalloc(goodMallocSize(head_.size)));
            boost::asio::async_read(socket_, boost::asio::buffer(buf, head_.size),
                std::bind(&TcpConnection::HandleReadContent, this, _1, _2, buf));
        }
        else // empty content packet for heartbeating
        {
            last_recv_time_ = getNowTickCount();
            stats_.total_recv_size += bytes;
            stats_.total_recv_count++;
            AsynRead();
        }
    }
    else
    {
        on_error_(err);
    }
}

void TcpConnection::HandleReadContent(const boost::system::error_code& ec,
                                      size_t bytes,
                                      uint8_t* buf)
{
    boost::system::error_code err = ec;
    if (!err && CheckContent(err, buf, bytes))
    {
        on_read_(serial_, ByteRange((const uint8_t*)buf, bytes));
        AsynRead();
        stats_.total_recv_size += bytes + sizeof(head_);
        stats_.total_recv_count++;
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


void TcpConnection::AsynWrite(const void* data, uint32_t size)
{
    if (data && size > 0 && size <= MAX_CONTENT_LEN)
    {
        Header head = { size, 0, 0 };
        head.size_checksum = crc32c((const uint8_t*)&head.size, sizeof(head.size));
        head.content_checksum = crc32c((const uint8_t*)data, size);
        size_t buf_size = size + sizeof(head);
        uint8_t* buf = (uint8_t*)checkedMalloc(goodMallocSize(buf_size));
        memcpy(buf, &head, sizeof(head));
        memcpy(buf + sizeof(head), data, size);
        boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
            std::bind(&TcpConnection::HandleWrite, this, _1, _2, buf));
    }    
}

void TcpConnection::HandleWrite(const boost::system::error_code& err,
                                size_t bytes,
                                uint8_t* buf)
{
    if (err)
    {
        on_error_(err);
    }
    else
    {
        stats_.total_send_size += bytes;
        stats_.total_send_count++;
    }
    free(buf);
}

bool TcpConnection::CheckHeader(boost::system::error_code& err, size_t bytes)
{
    err = boost::asio::error::invalid_argument;
    if (bytes == sizeof(head_))
    {
        auto checksum = crc32c((const uint8_t*)&head_.size, sizeof(head_.size));
        if (checksum == head_.size_checksum)
        {
            if (head_.size >= 0 && head_.size <= MAX_CONTENT_LEN)
            {
                return true;
            }
            else
            {
                LOG(ERROR) << serial_ << ", invalid content size: " << head_.size;
            }
        }
        else
        {
            LOG(ERROR) << serial_ << ", invalid header checksum: " << checksum 
                << ", " << head_.size_checksum;
        }
    }
    return false;
}

bool TcpConnection::CheckContent(boost::system::error_code& err, const uint8_t* buf, size_t bytes)
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
            LOG(ERROR) << serial_ << ", invalid content checksum: " << checksum 
                << ", " << head_.content_checksum;
        }
    }
    return false;
}

void TcpConnection::UpdateTransferStats()
{
    auto flag = 0x100000; // 1k
    if ((stats_.total_recv_size & flag) / flag > 0)
    {
        uint64_t now = getNowTickCount();
        uint64_t elapsed = (now - start_recv_time_) / 1000000000U;
        uint32_t packet_freq = to<uint32_t>(stats_.total_recv_count / elapsed);
        uint64_t size_freq = stats_.total_recv_size / elapsed;
        if (packet_freq > stats_.peak_recv_num_per_sec)
        {
            stats_.peak_recv_num_per_sec = packet_freq;
        }
        if (size_freq > stats_.peak_recv_size_per_sec)
        {
            stats_.peak_recv_size_per_sec = size_freq;
        }
    }
}
