#include "tcp_server.h"
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>


using std::cout;
using std::endl;

namespace arkto {

TCPServer::TCPServer(const std::string& address, 
                     const std::string& port,
                     AcceptHandler  on_accept,
                     ErrorHandler   on_error,
                     ReadHandler    on_read,
                     WriteHandler   after_write)
    : acceptor_(io_service_),
      current_serial_(0),
      on_accept_(on_accept),
      on_error_(on_error),
      on_read_(on_read),
      after_write_(after_write)
{
    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    TCPResolver resolver(io_service_);
    TCPResolver::query query(address, port);
    TCPEndpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(TCPAcceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    StartAccept();
}

TCPServer::~TCPServer()
{
}

void TCPServer::Run()
{
    ThreadPtr thrd_ptr(new boost::thread(boost::bind(&TCPServer::ProcessCommands, this)));
    thread_pool_.push_back(thrd_ptr);

    // worker threads
    size_t thread_pool_size = boost::thread::hardware_concurrency();
    for (size_t i = 0; i < thread_pool_size; ++i)
    {
        ThreadPtr thrd_ptr(new boost::thread(boost::bind(&IOService::run, &io_service_)));
        thread_pool_.push_back(thrd_ptr);
    }
}

void TCPServer::PushCommand(const Command& cmd)
{
    command_queue_.push(cmd);
}

void TCPServer::Close(int serial)
{
    auto iter = connections_.find(serial);
    if (iter != connections_.end())
    {
        auto conn = iter->second;
        conn->Stop();
        connections_.erase(iter);        
    }
    else
    {
        // log
    }
}

void TCPServer::AsynSend(int serial, const char* data, size_t size)
{
    auto iter = connections_.find(serial);
    if (iter != connections_.end())
    {
        auto conn = iter->second;
        conn->AsynSend(data, size);
    }
    else
    {
        // log
    }
}

void TCPServer::Broadcast(const char* data, size_t size)
{
    for (auto iter = connections_.begin(); iter != connections_.end(); ++iter)
    {
        auto conn = iter->second;
        conn->AsynSend(data, size);
    }
}


void TCPServer::StartAccept()
{
    int serial = ++current_serial_;
    new_connection_.reset(new TCPConnection(serial, *this, io_service_));
    acceptor_.async_accept(new_connection_->Socket(), 
        boost::bind(&TCPServer::HandleAccept, this, serial, _1));
}

// This method is called by worker thread
void TCPServer::HandleAccept(int serial, const ErrorCode& e)
{
    if (!e)
    {
        Command accept_cmd = {kCmdAccept, serial, e.value()};
        command_queue_.push(accept_cmd);
    }
    else
    {
        fprintf(stdout, e.message().c_str());
    }    
}


void TCPServer::HandleStop()
{
    io_service_.stop();
}

void TCPServer::OnAccept(int serial, int error)
{
    TCPEndpoint remote = new_connection_->Socket().remote_endpoint();
    std::string address = remote.address().to_string();
    if (on_accept_(serial, address.c_str()))
    {
        connections_[serial] = new_connection_;
        new_connection_->StartRead();
    }
    else
    {
        new_connection_.reset();
    }
    StartAccept();
}

void TCPServer::OnRead()
{

}

void TCPServer::OnClose()
{

}

void TCPServer::ProcessCommands()
{
    for (;;)
    {
        CommandList cmds;
        command_queue_.consume(cmds);
        if (cmds.empty())
        {
            boost::this_thread::yield();
            continue;
        }
        for (CommandList::iterator iter = cmds.begin(); 
            iter != cmds.end(); ++iter)
        {
            Command& cmd = *iter;
            switch(cmd.type_)
            {
            case kCmdAccept:
                {
                    OnAccept(cmd.serial_, cmd.error_);
                }
                break;
            case kCmdRecv:
                {
                    OnRead();
                }
                break;
            case kCmdClose:
                {
                    OnClose();
                }
                break;
            }
        }
    }    
}

} // namespace arkto
