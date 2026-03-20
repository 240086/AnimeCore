#include "network/session/Session.h"
#include "network/Connection.h"

Session::Session(uint64_t sessionId)
    : session_id_(sessionId) {
    last_heartbeat_ = std::chrono::steady_clock::now();
}

uint64_t Session::GetSessionId() const { return session_id_; }

void Session::BindConnection(std::shared_ptr<Connection> conn) {
    connection_ = conn;
}

std::shared_ptr<Connection> Session::GetConnection() {
    return connection_.lock();
}

void Session::UpdateHeartbeat() {
    last_heartbeat_ = std::chrono::steady_clock::now();
}

std::chrono::steady_clock::time_point Session::GetLastHeartbeat() const {
    return last_heartbeat_;
}