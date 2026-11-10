/**
 * @file yaml.cpp
 * @brief Exception-free facade over yaml-cpp's parse/emit.
 */

#include "yaml.hpp"
#include <fstream>

std::optional<YamlNode> Yaml::load(const char* path)
{
    try
    {
        return YAML::LoadFile(path);
    }
    catch (...)
    {
        return std::nullopt;
    }
}

std::optional<YamlNode> Yaml::load_text(const char* text)
{
    try
    {
        return YAML::Load(text);
    }
    catch (...)
    {
        return std::nullopt;
    }
}

bool Yaml::save(const YamlNode* root, const char* path)
{
    try
    {
        YamlNode copy = YAML::Clone(*root);
        copy.SetStyle(YAML::EmitterStyle::Flow);
        std::ofstream out(path);
        out << YAML::Dump(copy);
        return out.good();
    }
    catch (...)
    {
        return false;
    }
}

std::optional<bool> Yaml::get_bool(const YamlNode* node)
{
    try { return node->as<bool>(); }
    catch (...) { return std::nullopt; }
}

std::optional<f32> Yaml::get_float(const YamlNode* node)
{
    try { return node->as<f32>(); }
    catch (...) { return std::nullopt; }
}

std::optional<i32> Yaml::get_int(const YamlNode* node)
{
    try { return node->as<i32>(); }
    catch (...) { return std::nullopt; }
}

std::optional<std::string> Yaml::get_string(const YamlNode* node)
{
    try { return node->as<std::string>(); }
    catch (...) { return std::nullopt; }
}