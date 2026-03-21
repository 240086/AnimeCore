#pragma once

#include "network/buffer/RecvBuffer.h"
#include <boost/asio/detail/socket_ops.hpp>
#include <functional>

class InternalPacketParser
{
public:
    using Callback = std::function<void(uint32_t, uint16_t, const char *, size_t)>;

    void Parse(RecvBuffer &buffer, Callback cb);

private:
    static constexpr size_t HEADER_SIZE = 10; // 4+4+2
};