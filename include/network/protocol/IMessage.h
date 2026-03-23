#pragma once

#include <cstdint>
#include <vector>

namespace anime // 💡 推荐：使用项目缩写作为顶级命名空间
{
    // 消息来源类型，用于业务分流
    enum class MessageType
    {
        CLIENT,  // 6字节头包
        INTERNAL // 16字节头包
    };

    class IMessage
    {
    public:
        virtual ~IMessage() = default;

        // 1. 业务逻辑标识
        virtual uint16_t GetMsgId() const = 0;

        // 2. 识别包的协议属性
        virtual MessageType GetType() const = 0;

        // 3. 🔥 性能核心：允许直接获取二进制 Body
        virtual const char *GetData() const = 0;
        virtual size_t GetDataLen() const = 0;
    };
} // namespace anime