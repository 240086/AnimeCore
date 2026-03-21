#pragma once

#include <cstdint>
#include <vector>
#include <cstring>
#include <boost/asio/detail/socket_ops.hpp>

#pragma pack(push, 1)
struct InternalPacketHeader
{
    uint32_t length; // body + sid + msgId
    uint32_t sessionId;
    uint16_t messageId;
    uint16_t flags;
};
#pragma pack(pop)

class InternalPacket
{
public:
    void SetSessionId(uint32_t sid) { header_.sessionId = sid; }
    void SetMessageId(uint16_t id) { header_.messageId = id; }

    void Append(const char *data, size_t len);

    std::vector<char> Serialize() const;

private:
    InternalPacketHeader header_{};
    std::vector<char> body_;
};