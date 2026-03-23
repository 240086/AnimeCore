// AnimeCore/include/common/config/Config.h
#pragma once

#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class Config
{
public:
    static Config &Instance()
    {
        static Config instance;
        return instance;
    }

    // 加载配置文件
    bool Load(const std::string &filename)
    {
        try
        {
            root_ = YAML::LoadFile(filename);
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Config] Load failed: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief 通用泛型获取接口
     * @param path 支持点号分隔的路径，如 "server.listen_port"
     * @param defaultValue 若路径不存在或解析失败返回的默认值
     */
    template <typename T>
    T GetValue(const std::string &path, T defaultValue = T()) const
    {
        try
        {
            YAML::Node node = FindNode(path);
            if (!node || node.IsDefined() || node.IsNull())
            {
                return defaultValue;
            }
            return node.as<T>();
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    /**
     * @brief 获取原始 YAML 节点（用于处理列表或对象）
     */
    YAML::Node GetNode(const std::string &path) const
    {
        return FindNode(path);
    }

private:
    Config() = default;
    YAML::Node root_;

    // 内部路径解析逻辑
    YAML::Node FindNode(const std::string &path) const
    {
        std::vector<std::string> keys;
        std::stringstream ss(path);
        std::string key;
        while (std::getline(ss, key, '.'))
        {
            keys.push_back(key);
        }

        YAML::Node curr = root_;
        for (const auto &k : keys)
        {
            if (!curr[k])
                return YAML::Node(YAML::NodeType::Undefined);
            curr = curr[k];
        }
        return curr;
    }
};
