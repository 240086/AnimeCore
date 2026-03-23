#pragma once

#include <vector>
#include <memory>
#include "network/buffer/RecvBuffer.h"
#include "network/protocol/IMessage.h"

class PacketParser
{
public:
    virtual ~PacketParser() = default;

    // 🔥 核心接口：解析 → IMessage
    virtual void Parse(RecvBuffer &buffer,
                       std::vector<std::shared_ptr<IMessage>> &out) = 0;
};