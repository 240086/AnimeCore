#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

class RecvBuffer
{
public:
    void Append(const char *data, size_t len);

    size_t Size() const;

    const char *Data() const;

    void Consume(size_t len);

    void Clear()
    {
        buffer_.clear();
        buffer_.shrink_to_fit(); // 可选：释放物理内存，防止恶意超大包长期占用
    }

private:
    std::vector<char> buffer_;
    size_t read_index_ = 0;
};