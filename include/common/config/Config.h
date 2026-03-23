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

    bool Load(const std::string &filename)
    {
        try
        {
            // 明确清除旧数据并加载新文件
            root_ = YAML::LoadFile(filename);
            if (!root_.IsDefined())
                return false;
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
     */
    template <typename T>
    T GetValue(const std::string &path, T defaultValue = T()) const
    {
        try
        {
            YAML::Node node = FindNode(path);
            // ✅ 修正：只有当节点“未定义”或者是“空”时，才返回默认值
            if (!node.IsDefined() || node.IsNull())
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
     * @brief 获取原始 YAML 节点
     */
    YAML::Node GetNode(const std::string &path) const
    {
        return FindNode(path);
    }

private:
    Config() = default;
    YAML::Node root_;

    // ✅ 优化后的路径解析逻辑
    YAML::Node FindNode(const std::string &path) const
    {
        if (path.empty())
            return root_;

        std::stringstream ss(path);
        std::string key;
        YAML::Node curr = YAML::Clone(root_); // 使用 Clone 确保查找状态独立

        while (std::getline(ss, key, '.'))
        {
            // 检查当前 key 是否存在
            if (!curr[key])
            {
                return YAML::Node(); // 返回一个 Undefined 节点
            }
            curr = curr[key];
        }
        return curr;
    }
};