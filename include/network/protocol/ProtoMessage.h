#pragma once

#include "network/protocol/IMessage.h"
#include <memory>
#include <vector>
#include <string>

template <typename T>
class ProtoMessage : public anime::IMessage
{
public:
    ProtoMessage(uint16_t msgId, std::shared_ptr<T> pb)
        : msgId_(msgId), pb_(std::move(pb)) {}

    // --- 实现 IMessage 接口 ---

    uint16_t GetMsgId() const override { return msgId_; }

    // 💡 标识该消息来源于客户端协议
    anime::MessageType GetType() const override { return anime::MessageType::CLIENT; }

    /**
     * @brief 获取二进制 Body 数据 (懒加载实现)
     * 用于网关转发或日志审计，避免在业务逻辑中过早序列化
     */
    const char *GetData() const override
    {
        if (serialized_cache_.empty() && pb_)
        {
            SerializeToCache();
        }
        return serialized_cache_.data();
    }

    size_t GetDataLen() const override
    {
        if (serialized_cache_.empty() && pb_)
        {
            SerializeToCache();
        }
        return serialized_cache_.size();
    }

    // --- 业务层便捷接口 ---

    const T &Get() const { return *pb_; }
    T &Mutable() { return *pb_; }
    std::shared_ptr<T> GetPtr() const { return pb_; }

private:
    /**
     * @brief 将内部 PB 对象序列化并缓存到内存中
     */
    void SerializeToCache() const
    {
        // 使用 mutable 成员以支持 const 函数中的懒加载
        serialized_cache_.resize(pb_->ByteSizeLong());
        if (!pb_->SerializeToArray(const_cast<char *>(serialized_cache_.data()), serialized_cache_.size()))
        {
            serialized_cache_.clear();
        }
    }

private:
    uint16_t msgId_;
    std::shared_ptr<T> pb_;

    // 💡 缓存序列化后的数据，确保 GetData() 的性能
    mutable std::vector<char> serialized_cache_;
};