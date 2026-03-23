#include "network/protocol/ClientPacketParser.h"
#include "network/protocol/ClientPacket.h"
#include "common/logger/Logger.h"
#include <cstring>

void ClientPacketParser::Parse(RecvBuffer &buffer, std::vector<std::shared_ptr<IMessage>> &out)
{
    constexpr uint32_t MAX_PACKET_SIZE = 2 * 1024 * 1024; // 2MB 限制
    constexpr uint32_t HEADER_SIZE = 6;

    while (true)
    {
        // 1. 检查报头是否完整 (4字节长度 + 2字节MsgId)
        if (buffer.Size() < HEADER_SIZE)
            return;

        // 2. 预读长度（不移动读指针）
        uint32_t netLen;
        std::memcpy(&netLen, buffer.Data(), 4);
        uint32_t totalLen = ntohl(netLen);

        // 3. 安全检查
        if (totalLen < HEADER_SIZE || totalLen > MAX_PACKET_SIZE)
        {
            LOG_ERROR("[ClientParser] Invalid packet length: {}. Dropping connection.", totalLen);
            buffer.Consume(buffer.Size()); // 清空缓冲区防止攻击
            return;
        }

        // 4. 检查全包是否已到达
        if (buffer.Size() < totalLen)
            return;

        // 5. 提取 MsgId
        uint16_t netMsgId;
        std::memcpy(&netMsgId, buffer.Data() + 4, 2);
        uint16_t msgId = ntohs(netMsgId);

        // 6. 构造对象并转移数据
        auto packet = std::make_shared<ClientPacket>();
        packet->SetMessageId(msgId);

        size_t bodyLen = totalLen - HEADER_SIZE;
        if (bodyLen > 0)
        {
            std::vector<char> body(bodyLen);
            // 从 buffer 偏移 6 字节处开始读
            std::memcpy(body.data(), buffer.Data() + HEADER_SIZE, bodyLen);
            packet->SetBody(std::move(body));
        }

        // 7. 塞入输出队列，弹出已处理的缓冲区
        out.push_back(std::move(packet));
        buffer.Consume(totalLen);
    }
}