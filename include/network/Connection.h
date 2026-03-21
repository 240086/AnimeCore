#pragma once

#include <boost/asio.hpp>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <vector>

#include "network/buffer/RecvBuffer.h"
#include "network/protocol/Packet.h"
#include "network/protocol/PacketParser.h"

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    using tcp = boost::asio::ip::tcp;
    using strand_type = boost::asio::strand<boost::asio::io_context::executor_type>;
    using Task = std::function<void()>;

    struct Callbacks
    {
        // 收到完整业务包后触发（在连接所属 io_context 线程上执行）
        std::function<void(const std::shared_ptr<Connection> &, uint16_t, const char *, size_t)> onPacket;

        // 连接关闭后触发（在连接所属 io_context 线程上执行）
        std::function<void(const std::shared_ptr<Connection> &, uint64_t connectionId, uint64_t sessionId)> onClosed;

        // 需要异步清理 Session 资源时触发（由 cleanupExecutor 决定执行线程）
        std::function<void(uint64_t sessionId)> onSessionCleanup;
    };

    struct Options
    {
        size_t maxWriteQueueSize = 1024;
        // 若提供，将重清理任务投递到业务线程池/执行器
        std::function<void(Task)> cleanupExecutor;
    };

    explicit Connection(
        boost::asio::io_context &ioContext,
        Callbacks callbacks = {},
        Options options = {});

    tcp::socket &GetSocket();

    void Start();

    void SendPacket(const Packet &packet);

    void Close();

    void SetConnectionId(uint64_t id) { connection_id_ = id; }
    void SetSessionId(uint64_t id) { session_id_ = id; }

    uint64_t GetSessionId() const { return session_id_; }
    uint64_t GetConnectionId() const { return connection_id_; }

private:
    void DoRead();
    void DoWrite();
    void HandlePacket(uint16_t msgId, const char *data, size_t len);
    void DispatchCleanupTask(uint64_t sid);

private:
    tcp::socket socket_;
    strand_type strand_;

    enum
    {
        BUFFER_SIZE = 8192
    };

    char buffer_[BUFFER_SIZE]{};

    RecvBuffer recv_buffer_;
    PacketParser parser_;

    uint64_t session_id_ = 0;
    uint64_t connection_id_ = 0;

    std::chrono::steady_clock::time_point last_active_;

    std::atomic<bool> closed_{false};

    std::deque<std::shared_ptr<std::vector<char>>> write_queue_;

    Callbacks callbacks_;
    Options options_;
};
