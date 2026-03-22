#include "network/protocol/InternalPacketParser.h"
#include "common/logger/Logger.h"
#include <cstring>

void InternalPacketParser::Parse(RecvBuffer &buffer, Callback cb)
{
    // 工业级安全限制
    constexpr uint32_t MAX_PACKET_SIZE = 16 * 1024 * 1024; // 提升至 16MB (应对大数据包)
    // 最小长度 = HEADER_SIZE (因为我们的 Length 包含了 Header 本身)
    constexpr uint32_t MIN_PACKET_SIZE = HEADER_SIZE;

    while (true)
    {
        // 1. 首先检查是否够读取“长度字段”(4字节)
        if (buffer.Size() < 4)
            return;

        const char *data = buffer.Data();

        // 2. 解析总长度 (Network Order)
        uint32_t netLen;
        std::memcpy(&netLen, data, 4);
        uint32_t totalLen = ntohl(netLen);

        // ✅ 安全审计：防御性检查
        if (totalLen < MIN_PACKET_SIZE || totalLen > MAX_PACKET_SIZE)
        {
            LOG_ERROR("[Parser] Fatal protocol error. Invalid length: {}. Dropping buffer.", totalLen);
            buffer.Consume(buffer.Size()); // 协议已错乱，强制清空防止脏数据污染后续包
            return;
        }

        // 3. 检查当前缓冲区是否收全了整个包
        if (buffer.Size() < totalLen)
            return; // 数据不足，等待下一次 Read 事件

        // --- 4. 协议字段精准拆解 (严格对应 InternalPacketHeader 布局) ---
        uint32_t netSid;
        uint16_t netMsgId;
        uint32_t netSeqId;
        uint16_t netFlags;

        // 偏移量：
        // +0: Length (4)
        // +4: SessionId (4)
        // +8: MessageId (2)
        // +10: SequenceId (4)
        // +14: Flags (2)
        std::memcpy(&netSid, data + 4, 4);
        std::memcpy(&netMsgId, data + 8, 2);
        std::memcpy(&netSeqId, data + 10, 4);
        std::memcpy(&netFlags, data + 14, 2);

        uint32_t sid = ntohl(netSid);
        uint16_t msgId = ntohs(netMsgId);
        uint32_t seqId = ntohl(netSeqId);
        // uint16_t flags = ntohs(netFlags); // 预留字段，暂不处理

        // 5. 计算 Body 位置和长度
        size_t bodyLen = totalLen - HEADER_SIZE;
        const char *body = (bodyLen > 0) ? (data + HEADER_SIZE) : nullptr;

        // 6. 触发业务回调
        if (cb)
        {
            try
            {
                cb(sid, msgId, seqId, body, bodyLen);
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("[Parser] Callback exception: {}", e.what());
            }
        }

        // 7. 从环形缓冲区移除已处理数据
        buffer.Consume(totalLen);
    }
}