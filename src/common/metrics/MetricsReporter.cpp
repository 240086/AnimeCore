#include "common/metrics/Metrics.h"
#include "common/logger/Logger.h"
#include "MetricsReporter.h"

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

            LOG_INFO("====== METRICS ======");
            for (auto &kv : snap)
            {
                LOG_INFO("{} = {}", kv.first, kv.second);
            }

            Report();
        } });
}