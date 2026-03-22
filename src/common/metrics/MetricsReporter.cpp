#include "common/metrics/Metrics.h"
#include "common/logger/Logger.h"
#include "common/metrics/MetricsReporter.h"

MetricsReporter::MetricsReporter(boost::asio::io_context &io)
    : timer_(io)
{
}

void MetricsReporter::Start()
{
    Report();
}

void MetricsReporter::Report()
{
    timer_.expires_after(std::chrono::seconds(5));

    timer_.async_wait([this](const boost::system::error_code &ec)
                      {
        if (!ec)
        {
            auto snap = Metrics::Instance().Snapshot();

            LOG_INFO("==== METRICS ====");
            LOG_INFO("QPS = {}", snap[(size_t)MetricId::ProxyForwardTotal]);
            LOG_INFO("Reply = {}", snap[(size_t)MetricId::ProxyReplyTotal]);
            LOG_INFO("Timeout = {}", snap[(size_t)MetricId::RequestTimeout]);
            LOG_INFO("RateLimit = {}", snap[(size_t)MetricId::RateLimitHit]);

            LOG_INFO("Latency <1ms = {}", snap[(size_t)MetricId::Latency_0_1ms]);
            LOG_INFO("Latency 1~5ms = {}", snap[(size_t)MetricId::Latency_1_5ms]);
            LOG_INFO("Latency 5~10ms = {}", snap[(size_t)MetricId::Latency_5_10ms]);
            LOG_INFO("Latency 10~50ms = {}", snap[(size_t)MetricId::Latency_10_50ms]);
            LOG_INFO("Latency 50~100ms = {}", snap[(size_t)MetricId::Latency_50_100ms]);
            LOG_INFO("Latency >100ms = {}", snap[(size_t)MetricId::Latency_100ms_plus]);

            Report();
        } });
}