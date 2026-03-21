#pragma once
#include <atomic>
#include <unordered_map>
#include <string>
#include <mutex>

class Metrics
{
public:
    static Metrics &Instance();

    // -------- Counter --------
    void Inc(const std::string &key, uint64_t val = 1);

    uint64_t Get(const std::string &key);

    // -------- Gauge --------
    void Set(const std::string &key, uint64_t val);

    // -------- Snapshot（用于输出）--------
    std::unordered_map<std::string, uint64_t> Snapshot();

private:
    Metrics() = default;

private:
    std::mutex mtx_;
    std::unordered_map<std::string, std::atomic<uint64_t>> counters_;
};