#include "common/metrics/Metrics.h"

Metrics &Metrics::Instance()
{
    static Metrics instance;
    return instance;
}

std::array<uint64_t, static_cast<size_t>(MetricId::MAX_METRIC)> Metrics::Snapshot()
{
    std::array<uint64_t, static_cast<size_t>(MetricId::MAX_METRIC)> snap{};

    for (size_t i = 0; i < snap.size(); ++i)
    {
        snap[i] = counters_[i].load(std::memory_order_relaxed);
    }
    return snap;
}

void Metrics::ObserveLatency(uint64_t us)
{
    // us → bucket
    if (us < 1000)
        Inc(MetricId::Latency_0_1ms);
    else if (us < 5000)
        Inc(MetricId::Latency_1_5ms);
    else if (us < 10000)
        Inc(MetricId::Latency_5_10ms);
    else if (us < 50000)
        Inc(MetricId::Latency_10_50ms);
    else if (us < 100000)
        Inc(MetricId::Latency_50_100ms);
    else
        Inc(MetricId::Latency_100ms_plus);
}