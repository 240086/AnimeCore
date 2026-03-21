#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include <utility>
#include <vector>

struct GameServerNode
{
    std::string typeStr; // 存储字符串，如 "LOGIN"
    std::string host;
    int port;
    int connections;
};

class Config
{
public:
    static Config &Instance();

    bool Load(const std::string &file);

    int GetServerPort() const;
    int GetWorkerThreads() const;

    // --- 后端服务器集群 (新的) ---
    std::vector<GameServerNode> GetGameServers() const;

    // --- 路由逻辑 (关键) ---
    // 返回 pair: [min_id, max_id]
    std::pair<int, int> GetLoginRange() const;
    std::pair<int, int> GetGameRange() const;

    // --- 超时与保护 ---
    int GetBackendRequestTimeout() const;
    int GetClientIdleTimeout() const;

    std::string GetMysqlHost() const;
    int GetMysqlPort() const;
    std::string GetMysqlUser() const;     // 新增
    std::string GetMysqlPassword() const; // 新增
    std::string GetMysqlDatabase() const; // 新增
    int GetMysqlPoolSize() const;         // 新增

    std::string GetRedisHost() const;
    int GetRedisPort() const;
    int GetRedisPoolSize() const;

    std::string GetConfigDir() const;

    // --- 网关(Gateway) 核心配置 ---
    int GetGatewayPort() const;         // 网关监听端口（原 GetServerPort）
    std::string GetBackendHost() const; // 后端 Server IP
    int GetBackendPort() const;         // 后端 Server 端口
    int GetBackendPoolSize() const;     // 转发连接池大小

private:
    Config() = default;

    // 内部泛型工具：增加安全性
    template <typename T>
    T GetValue(const std::vector<std::string> &path, T defaultValue) const;

    YAML::Node root;
};