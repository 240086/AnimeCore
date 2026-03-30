#pragma once

#include <memory>

// 1. 必须在包含 spdlog.h 之前定义 ACTIVE_LEVEL
// 设置为 TRACE 表示 spdlog 的宏在代码层面是开启的，具体的过滤由我们下方的宏逻辑控制
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 
#include <spdlog/spdlog.h>

class Logger
{
public:
    static void Init();
    static std::shared_ptr<spdlog::logger>& GetLogger();

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

// --- 始终保留的日志 (INFO 及以上) ---
#define LOG_INFO(...)  SPDLOG_LOGGER_INFO(Logger::GetLogger(), __VA_ARGS__)
#define LOG_WARN(...)  SPDLOG_LOGGER_WARN(Logger::GetLogger(), __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(Logger::GetLogger(), __VA_ARGS__)
#define LOG_FATAL(...) SPDLOG_LOGGER_CRITICAL(Logger::GetLogger(), __VA_ARGS__)

// --- 仅在 DEBUG 环境下生效的宏 ---
#if defined(DEBUG) || !defined(NDEBUG)
    // 使用 SPDLOG_LOGGER_DEBUG 宏，它能自动捕获文件名和行号
    #define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(Logger::GetLogger(), __VA_ARGS__)
    #define LOG_TRACE(...) SPDLOG_LOGGER_TRACE(Logger::GetLogger(), __VA_ARGS__)
#else
    // Release 模式下，直接定义为空操作，编译器会完全移除相关代码
    #define LOG_DEBUG(...) (void)0
    #define LOG_TRACE(...) (void)0
#endif