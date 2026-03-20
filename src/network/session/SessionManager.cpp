#include "network/session/SessionManager.h"
#include "network/Connection.h"
#include "common/logger/Logger.h"

SessionManager &SessionManager::Instance()
{
    static SessionManager instance;
    return instance;
}

std::shared_ptr<Session> SessionManager::CreateSession()
{
    uint64_t id = next_session_id_++;
    auto session = std::make_shared<Session>(id);
    size_t idx = GetBucketIndex(id);
    {
        std::lock_guard<std::mutex> lock(buckets_[idx].mutex);
        buckets_[idx].sessions[id] = session;
    }
    return session;
}

void SessionManager::RemoveSession(uint64_t id)
{
    size_t idx = GetBucketIndex(id);
    {
        std::lock_guard<std::mutex> lock(buckets_[idx].mutex);
        buckets_[idx].sessions.erase(id);
    }
}

void SessionManager::CheckTimeout()
{
    auto now = std::chrono::steady_clock::now();
    for (size_t i = 0; i < BUCKET_COUNT; ++i)
    {
        std::lock_guard<std::mutex> lock(buckets_[i].mutex);
        for (auto it = buckets_[i].sessions.begin(); it != buckets_[i].sessions.end();)
        {
            // 如果 180 秒没心跳，直接关掉连接
            if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second->GetLastHeartbeat()).count() > 180)
            {
                if (auto conn = it->second->GetConnection())
                {
                    conn->Close();
                }
                it = buckets_[i].sessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}