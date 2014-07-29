#include "TcpConnection.h"
#include <boost/asio.hpp>
#include <functional>
#include "core/Conv.h"
#include "core/Malloc.h"
#include "core/Strings.h"
#include "net/Checksum.h"
#include "logging.h"
#include "Utils.h"


using namespace std;
using namespace std::placeholders;


TcpConnection::TcpConnection(boost::asio::io_service& io_service,
                             Serial serial,
                             ReadCallback on_read)
    : socket_(io_service),
      serial_(serial),
      on_read_(on_read)
{
    assert(on_read);
    last_recv_time_ = time(NULL);
}

TcpConnection::~TcpConnection()
{
    Close();
}

void TcpConnection::Close()
{
    if (!closed_ && socket_.is_open())
    {
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
        socket_.close();
        closed_ = true;
    }
}

void TcpConnection::AsynRead()
{
    boost::asio::async_read(socket_, boost::asio::buffer(&head_, sizeof(head_)),
        std::bind(&TcpConnection::HandleReadHead, this, _1, _2));
}

void TcpConnection::HandleReadHead(const boost::system::error_code& ec, size_t bytes)
{
    if (!ec)
    {
        // empty content packet for heartbeating
        if (head_.size == 0)
        {
            AsynRead();
            last_recv_time_ = getNowTickCount();
            UpdateTransferStats(bytes, 0);
        }
        else if (head_.size <= MAX_CONTENT_LEN)
        {
            if (recv_buf_.size() < head_.size)
            {
                recv_buf_.resize(head_.size);
            }
            boost::asio::async_read(socket_, boost::asio::buffer(recv_buf_.data(), head_.size),
                std::bind(&TcpConnection::HandleReadContent, this, _1, _2));
        }
        else
        {
            Close();
        }
    }
    else
    {
        Close();
        LOG(ERROR) << "Serial: " << serial_ << ", Error: " << ec.value()
            << ": " << ec.message();
    }
}

void TcpConnection::HandleReadContent(const boost::system::error_code& ec, size_t bytes)
{
    if (!ec)
    {
        const uint8_t* data = recv_buf_.data();
        if (CheckContent(data, bytes))
        {
            last_recv_time_ = getNowTickCount();
            on_read_(serial_, ByteRange(data, bytes));
            AsynRead();
            UpdateTransferStats(bytes + sizeof(head_), 0);
        }
        else
        {
            Close();
            LOG(ERROR) << "Serial: " << serial_ << ", Error: " << ec.value()
                << ": " << ec.message();
        }
    }
}


bool TcpConnection::AsynWrite(const void* data, uint32_t size)
{
    if (data == nullptr || size == 0 || size > MAX_CONTENT_LEN)
    {
        return false;
    }

    Header head = { (uint16_t)size };
    head.checksum = crc32c(data, size);
    size_t buf_size = size + sizeof(head);
    uint8_t* buf = (uint8_t*)checkedMalloc(goodMallocSize(buf_size));
    memcpy(buf, &head, sizeof(head));
    memcpy(buf + sizeof(head), data, size);
    boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
        std::bind(&TcpConnection::HandleWrite, this, _1, _2, buf));

    return true;
}

void TcpConnection::HandleWrite(const boost::system::error_code& err,
                                size_t bytes,
                                uint8_t* buf)
{
    if (err)
    {
        Close();
        LOG(ERROR) << "Serial: " << serial_ << ", Error: " << err.value() 
            << ": " << err.message();
    }
    else
    {
        UpdateTransferStats(0, bytes);
    }
    free(buf);
}

bool TcpConnection::CheckContent(const uint8_t* buf, size_t bytes)
{
    if (buf && bytes > 0 && bytes == head_.size)
    {
        auto checksum = crc32c(buf, bytes);
        if (checksum == head_.checksum)
        {
            return true;
        }
        else
        {
            LOG(ERROR) << serial_ << ", invalid content checksum: " << checksum
                << ", " << head_.checksum;
        }
    }
    return false;
}

void TcpConnection::UpdateTransferStats(size_t bytes_read, size_t bytes_send)
{
    if (bytes_read > 0)
    {
        stats_.total_recv_size += bytes_read;
        stats_.total_recv_count++;
    }
    if (bytes_send)
    {
        stats_.total_send_size += bytes_send;
        stats_.total_send_count++;
    }
}

void TcpConnection::UpdateTransferFreqency(int32_t sec)
{
    uint32_t packet_freq = (stats_.total_recv_count - last_recv_count_) / sec;
    float size_freq = (stats_.total_recv_size - last_recv_bytes_) / sec;
    if (packet_freq > stats_.peak_recv_num_per_sec)
    {
        stats_.peak_recv_num_per_sec = packet_freq;
    }
    if (size_freq > stats_.peak_recv_size_per_sec)
    {
        stats_.peak_recv_size_per_sec = size_freq;
    }
    last_recv_count_ = stats_.total_recv_count;
    last_recv_bytes_ = stats_.total_recv_size;
}
