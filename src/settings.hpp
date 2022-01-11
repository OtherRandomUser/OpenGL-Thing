#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

struct ObjectSettings
{
    std::string model;
    std::string texture;
};

struct SunSettings
{
    std::string model;
    std::string vertex_shader;
    std::string fragment_shader;
};

struct Settings
{
    std::string root_folder;
    std::string vertex_shader;
    std::string fragment_shader;
    std::vector<ObjectSettings> objects;
    SunSettings sun;
};

Settings load_settings(const std::string& filename);
