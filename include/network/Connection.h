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
#include "network/protocol/PacketParser.h"

using tcp = boost::asio::ip::tcp;
using strand_type = boost::asio::strand<boost::asio::io_context::executor_type>;
using Task = std::function<void()>;

class Connection;

struct Callbacks
{
    // ✅ 统一使用 IMessage 抽象
    std::function<void(
        const std::shared_ptr<Connection> &,
        std::shared_ptr<anime::IMessage>)>
        onPacket;

    std::function<void(const std::shared_ptr<Connection> &,
                       uint64_t connectionId,
                       uint64_t sessionId)>
        onClosed;

    std::function<void(uint64_t sessionId)> onSessionCleanup;
};

struct Options
{
    size_t maxWriteQueueSize = 1024;
    std::function<void(Task)> cleanupExecutor;
};

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    explicit Connection(
        boost::asio::io_context &ioContext,
        Callbacks callbacks = Callbacks{},
        Options options = Options{});

    tcp::socket &GetSocket();

    void Start();

    // 🔥 直接发送 raw（统一协议）
    void SendRaw(std::shared_ptr<std::vector<char>> data);

    void Close();

    void SetConnectionId(uint64_t id) { connection_id_ = id; }
    void SetSessionId(uint64_t id) { session_id_ = id; }

    uint64_t GetSessionId() const { return session_id_; }
    uint64_t GetConnectionId() const { return connection_id_; }

    void SetCallbacks(Callbacks cb)
    {
        callbacks_ = std::move(cb);
    }

    uint64_t GetLastActiveTimeUs() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   last_active_.time_since_epoch())
            .count();
    }

    uint32_t NextSeqId()
    {
        return seq_id_.fetch_add(1, std::memory_order_relaxed);
    }

    void SetParser(std::unique_ptr<PacketParser> parser)
    {
        parser_ = std::move(parser);
    }

private:
    void DoRead();
    void DoWrite();
    void DispatchCleanupTask(uint64_t sid);

private:
    tcp::socket socket_;
    strand_type strand_;

    static constexpr size_t BUFFER_SIZE = 8192;
    char buffer_[BUFFER_SIZE]{};

    RecvBuffer recv_buffer_;
    std::unique_ptr<PacketParser> parser_;

    uint64_t session_id_ = 0;
    uint64_t connection_id_ = 0;

    std::chrono::steady_clock::time_point last_active_;

    std::atomic<bool> closed_{false};

    std::deque<std::shared_ptr<std::vector<char>>> write_queue_;

    Callbacks callbacks_;
    Options options_;

    std::atomic<uint32_t> seq_id_{0};
};