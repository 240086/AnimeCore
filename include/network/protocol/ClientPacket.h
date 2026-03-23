// ClientPacket.h
#pragma once
#include "network/protocol/IMessage.h"
#include <vector>
#include <boost/asio/detail/socket_ops.hpp>

class ClientPacket : public IMessage
{
public:
    // 实现 IMessage 接口
    uint16_t GetMsgId() const override { return msgId_; }
    MessageType GetType() const override { return MessageType::CLIENT; }
    const char *GetData() const override { return body_.data(); }
    size_t GetDataLen() const override { return body_.size(); }

    // 业务接口
    void SetMessageId(uint16_t id) { msgId_ = id; }
    void SetBody(std::vector<char> &&body) { body_ = std::move(body); }

    std::vector<char> Serialize() const;

private:
    uint16_t msgId_ = 0;
    std::vector<char> body_;
};