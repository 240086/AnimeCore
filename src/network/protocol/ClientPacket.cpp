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