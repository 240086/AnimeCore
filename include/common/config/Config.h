// --- include/common/config/Config.h ---
#pragma once
#include <yaml-cpp/yaml.h>
#include <string>
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
            root_ = YAML::LoadFile(filename);
            return root_.IsDefined();
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Config] Load failed: " << e.what() << std::endl;
            return false;
        }
    }

    template <typename T>
    T GetValue(const std::string &path, T defaultValue = T()) const
    {
        try
        {
            YAML::Node node = FindNode(path);
            if (!node.IsDefined() || node.IsNull())
                return defaultValue;
            return node.as<T>();
        }
        catch (...)
        {
            return defaultValue;
        }
    }

private:
    Config() = default;
    YAML::Node root_;

    YAML::Node FindNode(const std::string &path) const
    {
        if (path.empty())
            return root_;
        std::stringstream ss(path);
        std::string key;
        YAML::Node curr = root_; // 直接使用引用语义，不要 Clone
        while (std::getline(ss, key, '.'))
        {
            if (!curr[key])
                return YAML::Node(); // 返回 Undefined
            curr = curr[key];
        }
        return curr;
    }
};