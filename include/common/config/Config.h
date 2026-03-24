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
        if (path.empty() || !root_.IsDefined())
            return root_;

        std::stringstream ss(path);
        std::string key;

        YAML::Node temp = root_;

        while (std::getline(ss, key, '.'))
        {
            if (key.empty())
                continue;

            if (temp.IsMap() && temp[key].IsDefined())
            {
                YAML::Node next = temp[key];
                temp = next;
            }
            else
            {
                return YAML::Node(YAML::NodeType::Undefined);
            }
        }
        return temp;
    }
};