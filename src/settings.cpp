#include <settings.hpp>

#include <fstream>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

void to_json(json& j, const ObjectSettings& s)
{
    j = json
    {
        {"model", s.model},
        {"texture", s.texture}
    };
}

void from_json(const json& j, ObjectSettings& s)
{
    j.at("model").get_to(s.model);
    j.at("texture").get_to(s.texture);
}

void to_json(json& j, const SunSettings& s)
{
    j = json
    {
        {"model", s.model},
        {"vertex-shader", s.vertex_shader},
        {"fragment-shader", s.fragment_shader}
    };
}

void from_json(const json& j, SunSettings& s)
{
    j.at("model").get_to(s.model);
    j.at("vertex-shader").get_to(s.vertex_shader);
    j.at("fragment-shader").get_to(s.fragment_shader);
}

void to_json(json& j, const Settings& s)
{
    j = json
    {
        {"root-folder", s.root_folder},
        {"vertex-shader", s.vertex_shader},
        {"fragment-shader", s.fragment_shader},
        {"objects", s.objects},
        {"sun", s.sun}
    };
}

void from_json(const json& j, Settings& s)
{
    j.at("root-folder").get_to(s.root_folder);
    j.at("vertex-shader").get_to(s.vertex_shader);
    j.at("fragment-shader").get_to(s.fragment_shader);
    j.at("objects").get_to(s.objects);
    j.at("sun").get_to(s.sun);
}

Settings load_settings(const std::string& filename)
{
    std::ifstream settings_file(filename);

    if (!settings_file.is_open())
    {
        spdlog::critical(
            "Unable to find settings file \"{}\"",
            filename);

        throw std::invalid_argument(fmt::format(
            "Unable to find settings file \"{}\"",
            filename));
    }

    json settings_json;
    settings_file >> settings_json;
    settings_file.close();

    Settings settings = settings_json;
    spdlog::info("successfuly loaded settings");

    return settings_json;
}
