#include "common/metrics/Metrics.h"

Metrics &Metrics::Instance()
{
    static Metrics instance;
    return instance;
}

void Metrics::Inc(const std::string &key, uint64_t val)
{
    std::lock_guard<std::mutex> lock(mtx_);
    counters_[key].fetch_add(val, std::memory_order_relaxed);
}

uint64_t Metrics::Get(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = counters_.find(key);
    if (it != counters_.end())
    {
        return it->second.load();
    }
    return 0;
}

void Metrics::Set(const std::string &key, uint64_t val)
{
    std::lock_guard<std::mutex> lock(mtx_);
    counters_[key].store(val);
}

std::unordered_map<std::string, uint64_t> Metrics::Snapshot()
{
    std::unordered_map<std::string, uint64_t> snap;

    std::lock_guard<std::mutex> lock(mtx_);
    for (auto &kv : counters_)
    {
        snap[kv.first] = kv.second.load();
    }
    return snap;
}