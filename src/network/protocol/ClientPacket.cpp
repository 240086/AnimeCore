#include "network/protocol/ClientPacket.h"

std::vector<char> ClientPacket::Serialize() const
{
    uint32_t totalLen = 4 + 2 + body_.size();

    std::vector<char> buf(totalLen);

    uint32_t netLen = htonl(totalLen);
    uint16_t netMsg = htons(msgId_);

    memcpy(buf.data(), &netLen, 4);
    memcpy(buf.data() + 4, &netMsg, 2);

    if (!body_.empty())
        memcpy(buf.data() + 6, body_.data(), body_.size());

    return buf;
}

void ClientPacket::Append(const char *data, size_t len)
{
    if (data && len > 0)
    {
        size_t oldSize = body_.size();
        body_.resize(oldSize + len);
        std::memcpy(body_.data() + oldSize, data, len);
    }
}

// 方便 std::string 接入
void ClientPacket::Append(const std::string &data)
{
    Append(data.data(), data.size());
}