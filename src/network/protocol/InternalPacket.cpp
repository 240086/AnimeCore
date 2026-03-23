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
    // 1. 确定头部大小 (16 字节)
    constexpr size_t HEADER_SIZE = 16;
    uint32_t bodyLen = static_cast<uint32_t>(body_.size());
    uint32_t totalLen = HEADER_SIZE + bodyLen;

    // 2. 预分配内存
    // 注意：std::vector<char> buf(totalLen) 会执行零初始化，
    // 如果对性能有极致要求，这里可以考虑使用自定义的 Buffer 类
    std::vector<char> buf(totalLen);
    char *ptr = buf.data();

    // 3. 字节序转换并填充 (手动偏移填充比多次 memcpy 稍微快一点点)
    // 这里建议直接操作内存，避免多余的变量创建
    *(uint32_t *)(ptr + 0) = htonl(totalLen);
    *(uint32_t *)(ptr + 4) = htonl(sessionId_); // 确保类成员名匹配
    *(uint16_t *)(ptr + 8) = htons(messageId_);
    *(uint32_t *)(ptr + 10) = htonl(sequenceId_);
    *(uint16_t *)(ptr + 14) = htons(flags_);

    // 4. 拷贝 Body
    if (bodyLen > 0)
    {
        std::memcpy(ptr + HEADER_SIZE, body_.data(), bodyLen);
    }

    return buf;
}