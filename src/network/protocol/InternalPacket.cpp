#include "network/protocol/InternalPacket.h"
#include <memory>

void InternalPacket::Append(const char *data, size_t len)
{
    if (!data || len == 0)
        return;

    // 1. 获取当前大小
    size_t currentSize = body_.size();

    // 2. 一次性调整大小（相比 push_back 更快）
    body_.reserve(body_.size() + len);

    // 3. 使用 std::memcpy 进行高速内存拷贝
    std::memcpy(body_.data() + currentSize, data, len);
}

std::vector<char> InternalPacket::Serialize() const
{
    constexpr size_t HEADER_SIZE = 10;

    uint32_t bodyLen = static_cast<uint32_t>(body_.size());
    uint32_t totalLen = bodyLen + 6;

    std::vector<char> sendBuf;
    sendBuf.resize(HEADER_SIZE + bodyLen);

    uint32_t netLen = htonl(totalLen);
    uint32_t netSid = htonl(header_.sessionId);
    uint16_t netMsgId = htons(header_.messageId);

    std::memcpy(sendBuf.data(), &netLen, 4);
    std::memcpy(sendBuf.data() + 4, &netSid, 4);
    std::memcpy(sendBuf.data() + 8, &netMsgId, 2);

    if (bodyLen > 0)
    {
        std::memcpy(sendBuf.data() + HEADER_SIZE, body_.data(), bodyLen);
    }

    return sendBuf;
}