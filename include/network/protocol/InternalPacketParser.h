#pragma once

#include "network/buffer/RecvBuffer.h"
#include <boost/asio/detail/socket_ops.hpp>
#include <functional>
#include <cstdint>

class InternalPacketParser
{
public:
    using Callback = std::function<void(uint32_t sid, uint16_t msgId, uint32_t seqId, const char *data, size_t len)>;

    void Parse(RecvBuffer &buffer, Callback cb);

private:
    // 🔥 必须与 InternalPacketHeader 严格对齐 (4+4+2+4+2 = 16)
    static constexpr size_t HEADER_SIZE = 16;
};