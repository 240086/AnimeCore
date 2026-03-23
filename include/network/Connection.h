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
#include "network/protocol/InternalPacketParser.h"

using tcp = boost::asio::ip::tcp;
using strand_type = boost::asio::strand<boost::asio::io_context::executor_type>;
using Task = std::function<void()>;

class Connection;

struct Callbacks
{
    // 🔥 升级：带 seqId
    std::function<void(const std::shared_ptr<Connection> &,
                       uint32_t sid,
                       uint16_t msgId,
                       uint32_t seqId,
                       const char *data,
                       size_t len)>
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
    InternalPacketParser parser_; // 🔥 替换

    uint64_t session_id_ = 0;
    uint64_t connection_id_ = 0;

    std::chrono::steady_clock::time_point last_active_;

    std::atomic<bool> closed_{false};

    std::deque<std::shared_ptr<std::vector<char>>> write_queue_;

    Callbacks callbacks_;
    Options options_;
};