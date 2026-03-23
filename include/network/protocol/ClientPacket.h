// ClientPacket.h
#pragma once
#include <vector>
#include <cstdint>
#include <boost/asio/detail/socket_ops.hpp>

class ClientPacket
{
public:
    void SetMessageId(uint16_t id) { msgId_ = id; }
    void Append(const char *data, size_t len)
    {
        body_.insert(body_.end(), data, data + len);
    }

    std::vector<char> Serialize() const;

private:
    uint16_t msgId_ = 0;
    std::vector<char> body_;
};