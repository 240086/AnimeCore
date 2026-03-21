#pragma once
#include <boost/asio.hpp>

class MetricsReporter
{
public:
    MetricsReporter(boost::asio::io_context &io);

    void Start();

private:
    void Report();

private:
    boost::asio::steady_timer timer_;
};