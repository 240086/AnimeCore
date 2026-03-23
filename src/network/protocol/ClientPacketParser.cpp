#include "network/protocol/ClientPacketParser.h"
#include "common/logger/Logger.h"
#include <cstring>

void ClientPacketParser::Parse(RecvBuffer &buffer, Callback cb)
{
    constexpr uint32_t MAX_PACKET_SIZE = 2 * 1024 * 1024; // 客户端包更严格
    constexpr uint32_t MIN_PACKET_SIZE = HEADER_SIZE;

    while (true)
    {
        if (buffer.Size() < 4)
            return;

        const char *data = buffer.Data();

        uint32_t netLen;
        std::memcpy(&netLen, data, 4);
        uint32_t totalLen = ntohl(netLen);

        if (totalLen < MIN_PACKET_SIZE || totalLen > MAX_PACKET_SIZE)
        {
            LOG_ERROR("[ClientParser] Invalid length {}", totalLen);
            buffer.Consume(buffer.Size());
            return;
        }

        if (buffer.Size() < totalLen)
            return;

        uint16_t netMsgId;
        std::memcpy(&netMsgId, data + 4, 2);

        uint16_t msgId = ntohs(netMsgId);

        size_t bodyLen = totalLen - HEADER_SIZE;
        const char *body = bodyLen ? data + HEADER_SIZE : nullptr;

        if (cb)
        {
            cb(msgId, body, bodyLen);
        }

        buffer.Consume(totalLen);
    }
}