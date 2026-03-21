#include "network/Connection.h"

#include "common/logger/Logger.h"

Connection::Connection(
    boost::asio::io_context &ioContext,
    Callbacks callbacks,
    Options options)
    : socket_(ioContext),
      strand_(boost::asio::make_strand(ioContext)),
      callbacks_(std::move(callbacks)),
      options_(std::move(options))
{
}

Connection::tcp::socket &Connection::GetSocket()
{
    return socket_;
}

void Connection::Start()
{
    last_active_ = std::chrono::steady_clock::now();
    DoRead();
}

void Connection::HandlePacket(uint16_t msgId, const char *data, size_t len)
{
    if (callbacks_.onPacket)
    {
        callbacks_.onPacket(shared_from_this(), msgId, data, len);
    }
}

void Connection::DoRead()
{
    auto self(shared_from_this());

    socket_.async_read_some(
        boost::asio::buffer(buffer_, sizeof(buffer_)),
        boost::asio::bind_executor(strand_,
                                   [this, self](boost::system::error_code ec, std::size_t length)
                                   {
                                       if (!ec)
                                       {
                                           last_active_ = std::chrono::steady_clock::now();
                                           recv_buffer_.Append(buffer_, length);

                                           parser_.Parse(
                                               recv_buffer_,
                                               [this](uint16_t msgId, const char *data, size_t len)
                                               {
                                                   HandlePacket(msgId, data, len);
                                               });

                                           DoRead();
                                       }
                                       else
                                       {
                                           if (ec != boost::asio::error::operation_aborted)
                                           {
                                               LOG_WARN("client read error: {} conn={}", ec.message(), connection_id_);
                                           }
                                           Close();
                                       }
                                   }));
}

void Connection::SendPacket(const Packet &packet)
{
    if (closed_.load())
        return;

    auto data = std::make_shared<std::vector<char>>(packet.Serialize());

    boost::asio::post(strand_, [this, self = shared_from_this(), data]()
                      {
        if (closed_.load()) return;

        if (write_queue_.size() > options_.maxWriteQueueSize)
        {
            LOG_WARN("drop packet due to queue limit, session={}", session_id_);
            return;
        }

        bool writing = !write_queue_.empty();
        write_queue_.push_back(data);

        if (!writing)
        {
            DoWrite();
        } });
}

void Connection::DoWrite()
{
    if (closed_.load() || write_queue_.empty())
        return;

    auto data = write_queue_.front();

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(*data),
        boost::asio::bind_executor(strand_,
                                   [this, self = shared_from_this()](boost::system::error_code ec, std::size_t)
                                   {
                                       if (ec)
                                       {
                                           Close();
                                           return;
                                       }

                                       if (!write_queue_.empty())
                                       {
                                           write_queue_.pop_front();
                                       }

                                       if (!write_queue_.empty() && !closed_.load())
                                       {
                                           DoWrite();
                                       }
                                   }));
}

void Connection::Close()
{
    if (closed_.exchange(true))
        return;

    boost::asio::post(strand_, [this, self = shared_from_this()]()
                      {
        write_queue_.clear();

        if (socket_.is_open())
        {
            boost::system::error_code ec;
            socket_.shutdown(tcp::socket::shutdown_both, ec);
            socket_.close(ec);
        }

        const uint64_t sid = session_id_;
        const uint64_t cid = connection_id_;

        if (callbacks_.onClosed)
        {
            callbacks_.onClosed(self, cid, sid);
        }

        if (sid != 0)
        {
            DispatchCleanupTask(sid);
            session_id_ = 0;
        } });
}

void Connection::DispatchCleanupTask(uint64_t sid)
{
    if (!callbacks_.onSessionCleanup)
    {
        return;
    }

    if (options_.cleanupExecutor)
    {
        options_.cleanupExecutor([handler = callbacks_.onSessionCleanup, sid]()
                                 { handler(sid); });
        return;
    }

    callbacks_.onSessionCleanup(sid);
}
