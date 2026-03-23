// InternalPacket.h (定型版)
#pragma once
#include "network/protocol/IMessage.h"
#include <boost/asio/detail/socket_ops.hpp>
#include <vector>

class InternalPacket : public anime::IMessage
{
public:
    InternalPacket() = default;

    InternalPacket(uint32_t sid,
                   uint16_t msgId,
                   uint32_t seqId)
        : sessionId_(sid),
          messageId_(msgId),
          sequenceId_(seqId)
    {
    }
    // --- 实现 IMessage 接口 ---
    uint16_t GetMsgId() const override { return messageId_; }
    anime::MessageType GetType() const override { return anime::MessageType::INTERNAL; }
    const char *GetData() const override { return body_.data(); }
    size_t GetDataLen() const override { return body_.size(); }

    // --- 内网特有元数据 ---
    uint32_t GetSessionId() const { return sessionId_; }
    uint32_t GetSequenceId() const { return sequenceId_; }
    uint16_t GetFlags() const { return flags_; }

    void SetSessionId(uint32_t sid) { sessionId_ = sid; }
    void SetMessageId(uint16_t id) { messageId_ = id; }
    void SetSequenceId(uint32_t seqId) { sequenceId_ = seqId; }
    void SetFlags(uint16_t flags) { flags_ = flags; }
    void SetBody(std::vector<char> &&body) { body_ = std::move(body); }

    void Append(const std::string &data);
    void Append(const char *data, size_t len);

    std::vector<char> Serialize() const;

private:
    uint32_t sessionId_ = 0;
    uint16_t messageId_ = 0;
    uint32_t sequenceId_ = 0;
    uint16_t flags_ = 0;
    std::vector<char> body_;
};