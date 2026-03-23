#include "network/protocol/InternalPacket.h"
#include <memory>

void InternalPacket::Append(const char *data, size_t len)
{
    if (!data || len == 0)
        return;

    // 1. 获取当前大小
    size_t currentSize = body_.size();

    // 2. 一次性调整大小（相比 push_back 更快）
    body_.resize(currentSize + len);

    // 3. 使用 std::memcpy 进行高速内存拷贝
    std::memcpy(body_.data() + currentSize, data, len);
}

void InternalPacket::Append(const std::string &data)
{
    Append(data.c_str(), data.length());
}

std::vector<char> InternalPacket::Serialize() const
{
    // 1. 动态计算 Header 大小，增强可维护性
    constexpr size_t HEADER_SIZE = sizeof(InternalPacketHeader); // 应该是 16 字节

    uint32_t bodyLen = static_cast<uint32_t>(body_.size());
    // 协议约定：length 通常指 [Header + Body] 总长度
    uint32_t totalLen = HEADER_SIZE + bodyLen;

    std::vector<char> sendBuf;
    sendBuf.resize(totalLen);

    // 2. 字节序转换 (Big-Endian / Network Order)
    uint32_t netLen = htonl(totalLen);
    uint32_t netSid = htonl(header_.sessionId);
    uint16_t netMsgId = htons(header_.messageId);
    uint32_t netSeqId = htonl(header_.sequenceId);
    uint16_t netFlags = htons(header_.flags);

    // 3. 高速内存拷贝到发送缓冲区
    char *ptr = sendBuf.data();

    std::memcpy(ptr, &netLen, 4);
    ptr += 4;
    std::memcpy(ptr, &netSid, 4);
    ptr += 4;
    std::memcpy(ptr, &netMsgId, 2);
    ptr += 2;
    std::memcpy(ptr, &netSeqId, 4);
    ptr += 4;
    std::memcpy(ptr, &netFlags, 2);
    ptr += 2;

    // 4. 拷贝 Body
    if (bodyLen > 0)
    {
        std::memcpy(ptr, body_.data(), bodyLen);
    }

    return sendBuf;
}