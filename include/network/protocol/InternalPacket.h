#pragma once

#include <cstdint>
#include <vector>
#include <cstring>
#include <boost/asio/detail/socket_ops.hpp>

#pragma pack(push, 1)
struct InternalPacketHeader
{
    uint32_t length;     // 总包长（通常不含自身4字节，或根据协议约定）
    uint32_t sessionId;  // 4字节
    uint16_t messageId;  // 2字节
    uint32_t sequenceId; // 4字节（🔥 核心新增）
    uint16_t flags;      // 2字节（扩展用，如压缩、加密标志）
};
#pragma pack(pop)

class InternalPacket
{
public:
    InternalPacket() : header_{0, 0, 0, 0, 0} {}

    // 允许构造时直接初始化核心字段，减少多次函数调用
    InternalPacket(uint32_t sid, uint16_t msgId, uint32_t seqId)
    {
        header_.sessionId = sid;
        header_.messageId = msgId;
        header_.sequenceId = seqId;
        header_.flags = 0;
    }

    void SetSessionId(uint32_t sid) { header_.sessionId = sid; }
    void SetMessageId(uint16_t id) { header_.messageId = id; }
    void SetSequenceId(uint32_t seqId) { header_.sequenceId = seqId; }
    uint32_t GetSequenceId() const { return header_.sequenceId; }

    void Append(const char *data, size_t len);

    // 序列化为字节流用于发送
    std::vector<char> Serialize() const;

private:
    InternalPacketHeader header_{};
    std::vector<char> body_;
};