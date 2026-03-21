#include "common/config/Config.h"

Config &Config::Instance()
{
    static Config instance;
    return instance;
}

bool Config::Load(const std::string &file)
{
    try
    {
        root = YAML::LoadFile(file);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

int Config::GetServerPort() const
{
    return root["server"]["port"].as<int>();
}

int Config::GetWorkerThreads() const
{
    return root["server"]["worker_threads"].as<int>();
}

std::string Config::GetMysqlHost() const
{
    return root["database"]["mysql_host"].as<std::string>();
}

int Config::GetMysqlPort() const
{
    return root["database"]["mysql_port"].as<int>();
}

std::string Config::GetMysqlUser() const
{
    return root["database"]["mysql_user"].as<std::string>("root");
}

std::string Config::GetMysqlPassword() const
{
    return root["database"]["mysql_pwd"].as<std::string>("");
}

std::string Config::GetMysqlDatabase() const
{
    return root["database"]["mysql_db"].as<std::string>("anime_game");
}

int Config::GetMysqlPoolSize() const
{
    return root["database"]["mysql_pool_size"].as<int>(10);
}

std::string Config::GetRedisHost() const
{
    return root["redis"]["host"].as<std::string>();
}

int Config::GetRedisPort() const
{
    return root["redis"]["port"].as<int>();
}

int Config::GetRedisPoolSize() const
{
    // 默认给 8 个，压测时可以根据硬件向上调整
    return root["redis"]["pool_size"].as<int>();
}

std::string Config::GetConfigDir() const
{
    return "config/";
}

int Config::GetGatewayPort() const
{
    return root["gateway"]["port"].as<int>(10000); // 默认 10000
}

std::string Config::GetBackendHost() const
{
    return root["backend"]["host"].as<std::string>("127.0.0.1");
}

int Config::GetBackendPort() const
{
    return root["backend"]["port"].as<int>(9000);
}

int Config::GetBackendPoolSize() const
{
    return root["backend"]["pool_size"].as<int>(4);
}

// 2. 解析服务器列表 (重点)
std::vector<GameServerNode> Config::GetGameServers() const
{
    std::vector<GameServerNode> servers;
    try
    {
        auto nodes = root["backend"]["game_servers"];
        if (nodes && nodes.IsSequence())
        {
            for (const auto &node : nodes)
            {
                servers.push_back({node["id"].as<int>(),
                                   node["host"].as<std::string>(),
                                   node["port"].as<int>(),
                                   node["connections"].as<int>(4)});
            }
        }
    }
    catch (...)
    {
        // 生产环境应记录日志
    }
    return servers;
}

// 3. 解析路由范围 [min, max]
std::pair<int, int> Config::GetLoginRange() const
{
    if (root["routing"]["login_range"])
    {
        auto range = root["routing"]["login_range"];
        return {range[0].as<int>(), range[1].as<int>()};
    }
    return {0, 999};
}

std::pair<int, int> Config::GetGameRange() const
{
    if (root["routing"]["game_range"])
    {
        auto range = root["routing"]["game_range"];
        return {range[0].as<int>(), range[1].as<int>()};
    }
    return {1000, 1999};
}

// 4. 超时配置
int Config::GetBackendRequestTimeout() const
{
    return root["timeout"]["backend_request_ms"].as<int>(1500);
}

int Config::GetClientIdleTimeout() const
{
    return root["timeout"]["client_idle_ms"].as<int>(60000);
}