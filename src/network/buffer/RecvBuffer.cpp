#include "network/buffer/RecvBuffer.h"

void RecvBuffer::Append(const char *data, size_t len)
{
    buffer_.insert(buffer_.end(), data, data + len);
}

size_t RecvBuffer::Size() const
{
    return buffer_.size();
}

const char *RecvBuffer::Data() const
{
    return buffer_.data() + read_index_;
}

void RecvBuffer::Consume(size_t len)
{
    read_index_ += len;
    // 关键：当已读空间太大时，才进行一次性重置（零摊还成本）
    if (read_index_ > buffer_.size() / 2)
    {
        buffer_.erase(buffer_.begin(), buffer_.begin() + read_index_);
        read_index_ = 0;
    }
}