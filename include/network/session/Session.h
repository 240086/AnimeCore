#pragma once
#include <memory>
#include <chrono>
#include <any> // C++17 提供的强大工具

class Connection;

class Session
{
public:
    Session(uint64_t sessionId);

    uint64_t GetSessionId() const;

    // 绑定连接
    void BindConnection(std::shared_ptr<Connection> conn);
    std::shared_ptr<Connection> GetConnection();

    // 🔥 核心重构：使用 std::any 替代具体的 Player
    // 这样 Session 就完全不需要知道 Player 的存在了
    void SetUserContext(std::any context) { user_context_ = std::move(context); }

    template <typename T>
    std::shared_ptr<T> GetUserContextAs()
    {
        try
        {
            return std::any_cast<std::shared_ptr<T>>(user_context_);
        }
        catch (const std::bad_any_cast &)
        {
            return nullptr;
        }
    }

    void UpdateHeartbeat();
    std::chrono::steady_clock::time_point GetLastHeartbeat() const;

private:
    uint64_t session_id_;
    std::weak_ptr<Connection> connection_;
    std::chrono::steady_clock::time_point last_heartbeat_;

    std::any user_context_; // 类型擦除，可以存任何东西
};