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
    constexpr size_t HEADER_SIZE = 16;
    uint32_t bodyLen = static_cast<uint32_t>(body_.size());
    uint32_t totalLen = HEADER_SIZE + bodyLen;

    std::vector<char> buf(totalLen);
    char *ptr = buf.data();

    // 安全填充，不使用指针强转
    uint32_t n_totalLen = htonl(totalLen);
    uint32_t n_sid = htonl(sessionId_);
    uint16_t n_mid = htons(messageId_);
    uint32_t n_seq = htonl(sequenceId_);
    uint16_t n_flags = htons(flags_);

    std::memcpy(ptr + 0, &n_totalLen, 4);
    std::memcpy(ptr + 4, &n_sid, 4);
    std::memcpy(ptr + 8, &n_mid, 2);
    std::memcpy(ptr + 10, &n_seq, 4);
    std::memcpy(ptr + 14, &n_flags, 2);

    if (bodyLen > 0)
    {
        std::memcpy(ptr + HEADER_SIZE, body_.data(), bodyLen);
    }
    return buf;
}