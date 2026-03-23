#pragma once

#include "network/protocol/PacketParser.h" // 必须包含基类
#include "network/protocol/IMessage.h"
#include <boost/asio/detail/socket_ops.hpp>

#include <vector>
#include <memory>

class ClientPacketParser : public PacketParser // ✅ 继承基类
{
public:
    ClientPacketParser() = default;
    virtual ~ClientPacketParser() = default;

    // ✅ 修改：删除旧的 Callback，对齐 .cpp 中的实现
    void Parse(RecvBuffer &buffer,
               std::vector<std::shared_ptr<anime::IMessage>> &out) override;

private:
    static constexpr size_t HEADER_SIZE = 6;
};