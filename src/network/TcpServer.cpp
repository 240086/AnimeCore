#include "network/TcpServer.h"

#include "common/logger/Logger.h"

TcpServer::TcpServer(
    boost::asio::io_context &mainContext,
    AsioContextPool &contextPool,
    int port,
    ConnectionFactory connectionFactory,
    AcceptCallback onAccepted)
    : mainContext_(mainContext),
      contextPool_(contextPool),
      acceptor_(mainContext),
      timer_(mainContext, std::chrono::seconds(30)),
      connectionFactory_(std::move(connectionFactory)),
      onAccepted_(std::move(onAccepted))
{
    tcp::endpoint endpoint(tcp::v4(), port);

    boost::system::error_code ec;

    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
        throw std::runtime_error("acceptor open failed");

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));

    acceptor_.bind(endpoint, ec);
    if (ec)
        throw std::runtime_error("acceptor bind failed");

    acceptor_.listen(8192, ec);
    if (ec)
        throw std::runtime_error("acceptor listen failed");

    if (!connectionFactory_)
    {
        connectionFactory_ = [](boost::asio::io_context &ioContext)
        {
            return std::make_shared<Connection>(ioContext, Callbacks{}, Options{});
        };
    }
}

void TcpServer::StartAccept()
{
    for (int i = 0; i < acceptorCount_; ++i)
    {
        DoAccept();
    }
}

void TcpServer::DoAccept()
{
    auto self = shared_from_this();
    auto &ioContext = contextPool_.GetIOContext();
    auto connection = connectionFactory_(ioContext);

    acceptor_.async_accept(
        connection->GetSocket(),
        [self, connection](boost::system::error_code ec)
        {
            if (!ec)
            {
                auto executor = connection->GetSocket().get_executor();

                boost::asio::post(executor, [self, connection]()
                                  {
                    connection->Start();

                    if (self->onAccepted_)
                    {
                        self->onAccepted_(connection);
                    }

                    const int accepted = self->acceptedCount_.fetch_add(1) + 1;
                    if ((accepted % 100) == 0)
                    {
                        LOG_INFO("client accepted total={}", accepted);
                    } });
            }
            else
            {
                LOG_ERROR("accept error {}", ec.message());
            }

            self->DoAccept();
        });
}

void TcpServer::Stop()
{
    boost::system::error_code ec;
    acceptor_.close(ec);
    timer_.cancel();
    LOG_INFO("TcpServer listener closed.");
}
