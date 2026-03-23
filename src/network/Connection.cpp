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

tcp::socket &Connection::GetSocket()
{
    return socket_;
}

void Connection::Start()
{
    last_active_ = std::chrono::steady_clock::now();
    DoRead();
}

void Connection::DoRead()
{
    auto self(shared_from_this());

    socket_.async_read_some(
        boost::asio::buffer(buffer_, sizeof(buffer_)),
        boost::asio::bind_executor(
            strand_,
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    last_active_ = std::chrono::steady_clock::now();

                    // 1. 将新数据推入接收缓冲区
                    recv_buffer_.Append(buffer_, length);

                    // 2. 准备一个容器接收解析出的消息对象
                    // 使用 shared_ptr 保证消息在分发过程中的生命周期安全
                    std::vector<std::shared_ptr<IMessage>> messages;

                    // 3. 🔥 核心变化：由 Parser 填充 messages 列表
                    // 此时 parser_ 是 std::unique_ptr<PacketParser>
                    if (parser_)
                    {
                        parser_->Parse(recv_buffer_, messages);
                    }

                    // 4. 遍历并分发消息
                    for (auto &msg : messages)
                    {
                        if (callbacks_.onPacket)
                        {
                            // 注意：这里的 callbacks_.onPacket 签名需要修改
                            // 变为：void(std::shared_ptr<Connection>, std::shared_ptr<IMessage>)
                            callbacks_.onPacket(self, msg);
                        }
                    }

                    // 5. 继续监听下一次读取
                    DoRead();
                }
                else
                {
                    if (ec != boost::asio::error::operation_aborted)
                    {
                        LOG_WARN("Connection read error: {} conn_id={}",
                                 ec.message(), connection_id_);
                    }
                    Close();
                }
            }));
}

void Connection::SendRaw(std::shared_ptr<std::vector<char>> data)
{
    if (closed_.load())
        return;

    boost::asio::post(strand_,
                      [this, self = shared_from_this(), data]()
                      {
                          if (closed_.load())
                              return;

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
                          }
                      });
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

                                       write_queue_.pop_front();

                                       if (!write_queue_.empty() && !closed_.load())
                                       {
                                           last_active_ = std::chrono::steady_clock::now();
                                           DoWrite();
                                       }
                                   }));
}

void Connection::Close()
{
    if (closed_.exchange(true))
        return;

    boost::asio::post(strand_,
                      [this, self = shared_from_this()]()
                      {
                          write_queue_.clear();

                          if (socket_.is_open())
                          {
                              boost::system::error_code ec;
                              socket_.shutdown(tcp::socket::shutdown_both, ec);
                              socket_.close(ec);
                          }

                          uint64_t sid = session_id_;
                          uint64_t cid = connection_id_;

                          if (callbacks_.onClosed)
                          {
                              callbacks_.onClosed(self, cid, sid);
                          }

                          if (sid != 0)
                          {
                              DispatchCleanupTask(sid);
                              session_id_ = 0;
                          }
                      });
}

void Connection::DispatchCleanupTask(uint64_t sid)
{
    if (!callbacks_.onSessionCleanup)
        return;

    if (options_.cleanupExecutor)
    {
        options_.cleanupExecutor(
            [handler = callbacks_.onSessionCleanup, sid]()
            {
                handler(sid);
            });
    }
    else
    {
        callbacks_.onSessionCleanup(sid);
    }
}