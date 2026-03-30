#include "common/logger/Logger.h"
#include "common/config/Config.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <filesystem>

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init()
{
    try
    {
        // 1. 目录检查
        std::filesystem::path log_path("logs");
        if (!std::filesystem::exists(log_path))
        {
            std::filesystem::create_directories(log_path);
        }

        // 2. 初始化异步线程池（8192 是队列长度，1 是后台写线程数）
        spdlog::init_thread_pool(8192, 1);

        // 3. 创建 Sinks
        // 控制台：带颜色，级别设为 trace 以便开发调试
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        
        // 文件：每日滚动，0点0分刷新
        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/server_log.txt", 0, 0);

        // --- 核心升级：设置 Pattern ---
        // [%s:%#] 会显示 "文件名:行号"，[%t] 显示线程 ID
        // %^...%$ 是 spdlog 的颜色控制符
        std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%t] [%n] [%^%l%$] [%s:%#] %v";
        console_sink->set_pattern(pattern);
        file_sink->set_pattern(pattern);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

        // 4. 从配置读取 Logger 名称
        auto name = Config::Instance().GetValue<std::string>("server.name", "AnimeGame");
        auto id = Config::Instance().GetValue<int>("server.id", 1);
        std::string loggerName = name + "-" + std::to_string(id);

        // 5. 创建异步 Logger
        s_Logger = std::make_shared<spdlog::async_logger>(
            loggerName,
            sinks.begin(),
            sinks.end(),
            spdlog::thread_pool(),
            // block 策略比 overrun 更好，因为它在日志量巨大时会保护内存不溢出
            spdlog::async_overflow_policy::block 
        );

        // 6. 根据编译模式设置全局级别
#if defined(DEBUG) || !defined(NDEBUG)
        s_Logger->set_level(spdlog::level::debug);
        console_sink->set_level(spdlog::level::debug);
#else
        s_Logger->set_level(spdlog::level::info);
        console_sink->set_level(spdlog::level::info);
#endif
        file_sink->set_level(spdlog::level::info); // 文件通常只存 info 以上

        // 7. 注册并配置刷盘策略
        spdlog::set_default_logger(s_Logger);
        s_Logger->flush_on(spdlog::level::warn);      // Warn 及以上立即刷盘
        spdlog::flush_every(std::chrono::seconds(3)); // 每 3 秒自动定期刷盘

        LOG_INFO("Logger initialized successfully. Mode: {}", 
#if defined(DEBUG) || !defined(NDEBUG)
            "Debug"
#else
            "Release"
#endif
        );
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::printf("FATAL: Logger Init Failed: %s\n", ex.what());
    }
}

std::shared_ptr<spdlog::logger> &Logger::GetLogger()
{
    return s_Logger;
}