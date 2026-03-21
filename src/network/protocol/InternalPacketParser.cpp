#include "network/protocol/InternalPacketParser.h"
#include "common/logger/Logger.h"

void InternalPacketParser::Parse(RecvBuffer &buffer, Callback cb)
{
    while (buffer.Size() >= HEADER_SIZE)
    {
        const char *data = buffer.Data();

        uint32_t netLen, netSid;
        uint16_t netMsgId;

        std::memcpy(&netLen, data, 4);
        uint32_t hostLen = ntohl(netLen);

        // 边界检查：防止恶意大包导致缓冲区溢出攻击
        if (hostLen > 1024 * 1024 * 8)
        { // 限制 8MB
            LOG_ERROR("Internal packet too large: {}", hostLen);
            buffer.Clear(); // 简单粗暴断开处理
            return;
        }

        if (buffer.Size() < hostLen + 4)
            return; // 数据包未完全到达

        std::memcpy(&netSid, data + 4, 4);
        std::memcpy(&netMsgId, data + 8, 2);

        uint32_t sid = ntohl(netSid);
        uint16_t msgId = ntohs(netMsgId);

        // 计算 Body 偏移
        const char *body = data + HEADER_SIZE;
        size_t bodyLen = hostLen - (HEADER_SIZE - 4); // 减去 Header 中除 Len 以外的长度

        if (cb)
            cb(sid, msgId, body, bodyLen);

        buffer.Consume(hostLen + 4);
    }
}