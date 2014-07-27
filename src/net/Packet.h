#pragma once

#include <cstdint>
#include <functional>
#include <boost/system/error_code.hpp>
#include "core/Range.h"

/**
 *  a network packet on the wire consist of :
 *
 *  +---------------+----------------------+
 *  |    Header     |     content          |
 *  +---------------+----------------------+
 *
 */

struct Header
{
    uint16_t     size;          // content size
    uint16_t     flag;          // compression type
    uint32_t     checksum;      // checksum value of content
};

enum
{
    // max content size, size of a packet is limited to 64k
    MAX_CONTENT_LEN = (64 * 1024) - sizeof(Header),
};

struct ServerOptions
{
    // heartbeat seconds
    uint16_t  heart_beat_sec = 100;

    // max connection allowed
    uint16_t  max_connections = 2000;

    // max recv packet per second for every connection
    uint32_t  max_recv_num_per_sec = 1000;

    // max recv size per second for every connection
    uint32_t  max_recv_size_per_sec = MAX_CONTENT_LEN * 60;
};

// transferred data stats
struct TransferStats
{
    // total packet count sended
    uint32_t    total_send_count = 0;

    // total packets count recv
    uint32_t    total_recv_count = 0;

    // total bytes sended
    float       total_send_size = 0;

    // total bytes recv
    float       total_recv_size = 0;

    // peak recv packet count per second
    uint32_t    peak_recv_num_per_sec = 0;

    // peak recv bytes per second
    float    peak_recv_size_per_sec = 0;
};

// Serial is an identifier for every TCP connection object, for a 
// singled-threaded TCP server, take 32 bit integer as type of Serial 
// is safe and easy to maintain. But if we want extend out design to 
// multi-threaded(or multi-process) model, this integer desgin will be
// deprecated, the UUID component is much better.
typedef uint32_t    Serial;


// error callback
typedef std::function<void(const boost::system::error_code)> ErrorCallback;

// read callback
typedef std::function<void(Serial, ByteRange)>     ReadCallback;
