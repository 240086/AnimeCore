#pragma once

#include "network/buffer/RecvBuffer.h"
#include <functional>
#include <cstdint>
#include <boost/asio/detail/socket_ops.hpp>

class ClientPacketParser
{
public:
    using Callback = std::function<void(uint16_t msgId,
                                        const char *data,
                                        size_t len)>;

    void Parse(RecvBuffer &buffer, Callback cb);

private:
    static constexpr size_t HEADER_SIZE = 6; // 4 + 2
};