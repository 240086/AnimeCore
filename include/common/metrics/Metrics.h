#pragma once
#include <atomic>
#include <array>
#include <cstdint>

enum class MetricId : uint32_t
{
    // ---- Counter ----
    ProxyForwardTotal,
    ProxyForwardFail,
    ProxyReplyTotal,

    RequestTimeout,
    RateLimitHit,

    // ---- Gauge ----
    ConnectionActive,

    // ---- Latency Bucket ----
    Latency_0_1ms,
    Latency_1_5ms,
    Latency_5_10ms,
    Latency_10_50ms,
    Latency_50_100ms,
    Latency_100ms_plus,

    MAX_METRIC
};

class Metrics
{
public:
    static Metrics &Instance();

    inline void Add(MetricId id, int64_t val)
    {
        counters_[static_cast<size_t>(id)].fetch_add(val, std::memory_order_relaxed);
    }
    // Counter
    inline void Inc(MetricId id, uint64_t val = 1)
    {
        counters_[static_cast<size_t>(id)].fetch_add(val, std::memory_order_relaxed);
    }

    // Gauge
    inline void Set(MetricId id, uint64_t val)
    {
        counters_[static_cast<size_t>(id)].store(val, std::memory_order_relaxed);
    }

    inline uint64_t Get(MetricId id)
    {
        return counters_[static_cast<size_t>(id)].load(std::memory_order_relaxed);
    }

    // Snapshot（无锁）
    std::array<uint64_t, static_cast<size_t>(MetricId::MAX_METRIC)> Snapshot();

    // Latency
    void ObserveLatency(uint64_t us);

private:
    Metrics() = default;

private:
    std::array<std::atomic<uint64_t>, static_cast<size_t>(MetricId::MAX_METRIC)> counters_{};
};