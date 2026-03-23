#pragma once

#include "network/protocol/PacketParser.h"
#include <vector>
#include <memory>

class InternalPacketParser : public PacketParser
{
public:
    InternalPacketParser() = default;
    virtual ~InternalPacketParser() = default;

    // ✅ 对齐接口
    void Parse(RecvBuffer &buffer,
               std::vector<std::shared_ptr<IMessage>> &out) override;

private:
    static constexpr size_t HEADER_SIZE = 16;
};