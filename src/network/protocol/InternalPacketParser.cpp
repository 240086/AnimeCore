#include "network/protocol/InternalPacketParser.h"
#include "common/logger/Logger.h"

void InternalPacketParser::Parse(RecvBuffer &buffer, Callback cb)
{
    constexpr uint32_t MAX_PACKET_SIZE = 8 * 1024 * 1024; // 8MB
    constexpr uint32_t MIN_PACKET_SIZE = 6;               // sid(4) + msgId(2)

    while (true)
    {
        if (buffer.Size() < HEADER_SIZE)
            return;

        const char *data = buffer.Data();

        uint32_t netLen;
        std::memcpy(&netLen, data, 4);

        uint32_t bodyTotalLen = ntohl(netLen);

        // ✅ 1. 最小长度检查（防协议攻击）
        if (bodyTotalLen < MIN_PACKET_SIZE)
        {
            LOG_ERROR("Invalid internal packet length: {}", bodyTotalLen);
            buffer.Consume(buffer.Size()); // 丢弃所有数据
            return;
        }

        // ✅ 2. 最大长度检查
        if (bodyTotalLen > MAX_PACKET_SIZE)
        {
            LOG_ERROR("Internal packet too large: {}", bodyTotalLen);
            buffer.Consume(buffer.Size());
            return;
        }

        // ✅ 3. 包完整性检查
        if (buffer.Size() < bodyTotalLen + 4)
            return;

        // --- 解析字段 ---
        uint32_t netSid;
        uint16_t netMsgId;

        std::memcpy(&netSid, data + 4, 4);
        std::memcpy(&netMsgId, data + 8, 2);

        uint32_t sid = ntohl(netSid);
        uint16_t msgId = ntohs(netMsgId);

        // ✅ 明确语义：body长度
        size_t bodyLen = bodyTotalLen - MIN_PACKET_SIZE;
        const char *body = data + HEADER_SIZE;

        if (cb)
        {
            cb(sid, msgId, body, bodyLen);
        }

        buffer.Consume(bodyTotalLen + 4);
    }
}