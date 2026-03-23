#include "network/protocol/InternalPacketParser.h"
#include "network/protocol/InternalPacket.h"
#include "common/logger/Logger.h"
#include <cstring>

void InternalPacketParser::Parse(RecvBuffer &buffer, std::vector<std::shared_ptr<IMessage>> &out)
{
    // 对应你的 16 字节头：4(Len)+4(SID)+2(MsgID)+4(SeqID)+2(Flags)
    constexpr size_t HEADER_SIZE = 16;
    constexpr uint32_t MAX_PACKET_SIZE = 16 * 1024 * 1024; // 16MB

    while (true)
    {
        // 1. 至少要能读到长度字段
        if (buffer.Size() < 4)
            return;

        // 2. 预读总长度
        uint32_t netLen;
        std::memcpy(&netLen, buffer.Data(), 4);
        uint32_t totalLen = ntohl(netLen);

        // 3. 安全检查
        if (totalLen < HEADER_SIZE || totalLen > MAX_PACKET_SIZE)
        {
            LOG_ERROR("[InternalParser] Fatal error. Invalid totalLen: {}", totalLen);
            buffer.Consume(buffer.Size()); // 丢弃所有数据
            return;
        }

        // 4. 等待全包
        if (buffer.Size() < totalLen)
            return;

        // 5. 精准拆解字段 (注意偏移量)
        const char *d = buffer.Data();

        uint32_t sid = ntohl(*(uint32_t *)(d + 4));
        uint16_t msgId = ntohs(*(uint16_t *)(d + 8));
        uint32_t seqId = ntohl(*(uint32_t *)(d + 10));
        uint16_t flags = ntohs(*(uint16_t *)(d + 14));

        // 6. 构造对象
        auto packet = std::make_shared<InternalPacket>();
        packet->SetSessionId(sid);
        packet->SetMessageId(msgId);
        packet->SetSequenceId(seqId);
        packet->SetFlags(flags);

        // 7. 提取 Body
        size_t bodyLen = totalLen - HEADER_SIZE;
        if (bodyLen > 0)
        {
            std::vector<char> body(bodyLen);
            std::memcpy(body.data(), d + HEADER_SIZE, bodyLen);
            packet->SetBody(std::move(body));
        }

        // 8. 产出并移除
        out.push_back(std::move(packet));
        buffer.Consume(totalLen);
    }
}