#pragma once

#include <boost/asio.hpp>
#include <atomic>
#include <functional>
#include <memory>

#include "network/Connection.h"
#include "network/asio/AsioContextPool.h"

class TcpServer : public std::enable_shared_from_this<TcpServer>
{
public:
    using tcp = boost::asio::ip::tcp;
    using ConnectionFactory = std::function<std::shared_ptr<Connection>(boost::asio::io_context &)>;
    using AcceptCallback = std::function<void(const std::shared_ptr<Connection> &)>;

    TcpServer(
        boost::asio::io_context &mainContext,
        AsioContextPool &contextPool,
        int port,
        ConnectionFactory connectionFactory = {},
        AcceptCallback onAccepted = {});

    void StartAccept();

    void Stop();

private:
    void DoAccept();

private:
    boost::asio::io_context &mainContext_;

    AsioContextPool &contextPool_;

    tcp::acceptor acceptor_;

    boost::asio::steady_timer timer_;

    ConnectionFactory connectionFactory_;
    AcceptCallback onAccepted_;

    int acceptorCount_ = 4;
    std::atomic<int> acceptedCount_{0};
};
