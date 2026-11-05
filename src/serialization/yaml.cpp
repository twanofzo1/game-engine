/**
 * @file yaml.cpp
 * @brief Exception-free facade over yaml-cpp's parse/emit.
 */
#include "serialization/yaml.hpp"

#include <fstream>
#include <string>

namespace Yaml {

bool load(Node* root, const char* path)
{
    try
    {
        *root = YAML::LoadFile(path);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool load_text(Node* root, const char* text)
{
    try
    {
        *root = YAML::Load(text);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

/** @brief Recursively mark scalar-only sequences to emit as flow collections. */
static void mark_flow_sequences(YAML::Node node)
{
    if (node.IsSequence())
    {
        bool scalar_only = true;
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            if (!it->IsScalar())
            {
                scalar_only = false;
                break;
            }
        }

        if (scalar_only)
        {
            node.SetStyle(YAML::EmitterStyle::Flow);
            return;
        }

        for (auto it = node.begin(); it != node.end(); ++it)
            mark_flow_sequences(*it);
    }
    else if (node.IsMap())
    {
        for (auto it = node.begin(); it != node.end(); ++it)
            mark_flow_sequences(it->second);
    }
}

bool save(const Node* root, const char* path)
{
    try
    {
        Node copy = YAML::Clone(*root);
        mark_flow_sequences(copy);
        std::ofstream out(path);
        out << YAML::Dump(copy);
        return out.good();
    }
    catch (...)
    {
        return false;
    }
}

i32 as_int(const Node& node, i32 fallback)
{
    try { return node.as<i32>(); }
    catch (...) { return fallback; }
}

f32 as_float(const Node& node, f32 fallback)
{
    try { return node.as<f32>(); }
    catch (...) { return fallback; }
}

bool as_bool(const Node& node, bool fallback)
{
    try { return node.as<bool>(); }
    catch (...) { return fallback; }
}

std::string as_string(const Node& node, const char* fallback)
{
    try
    {
        std::string value = node.as<std::string>();
        if (!value.empty() || fallback == NULL)
            return value;
    }
    catch (...)
    {
    }
    return fallback ? std::string(fallback) : std::string();
}

Engine::Vec2::Vec2 as_vec2(const Node& node, Engine::Vec2::Vec2 fallback)
{
    Engine::Vec2::Vec2 out;
    if (get_vec2(node, &out))
        return out;
    return fallback;
}

bool get_int(const Node& node, i32* out)
{
    if (out == NULL)
        return false;
    *out = 0;
    try { *out = node.as<i32>(); return true; }
    catch (...) { return false; }
}

bool get_float(const Node& node, f32* out)
{
    if (out == NULL)
        return false;
    *out = 0.0f;
    try { *out = node.as<f32>(); return true; }
    catch (...) { return false; }
}

bool get_bool(const Node& node, bool* out)
{
    if (out == NULL)
        return false;
    *out = false;
    try { *out = node.as<bool>(); return true; }
    catch (...) { return false; }
}

bool get_string(const Node& node, std::string* out)
{
    if (out == NULL)
        return false;
    out->clear();
    try { *out = node.as<std::string>(); return true; }
    catch (...) { return false; }
}

bool get_vec2(const Node& node, Engine::Vec2::Vec2* out)
{
    if (out == NULL)
        return false;
    *out = Engine::Vec2::make(0.0f, 0.0f);
    try
    {
        if (!node.IsSequence() || node.size() < 2)
            return false;
        *out = Engine::Vec2::make(node[0].as<f32>(), node[1].as<f32>());
        return true;
    }
    catch (...)
    {
        return false;
    }
}

Engine::Color::Color as_color(const Node& node, Engine::Color::Color fallback)
{
    Engine::Color::Color out;
    if (get_color(node, &out))
        return out;
    return fallback;
}

bool get_color(const Node& node, Engine::Color::Color* out)
{
    if (out == NULL)
        return false;
    *out = Engine::Color::make(0.0f, 0.0f, 0.0f, 0.0f);
    try
    {
        if (!node.IsSequence() || node.size() < 4)
            return false;
        *out = Engine::Color::make(node[0].as<f32>(), node[1].as<f32>(),
                                   node[2].as<f32>(), node[3].as<f32>());
        return true;
    }
    catch (...)
    {
        return false;
    }
}

Node make_vec2(Engine::Vec2::Vec2 value)
{
    Node node;
    node.SetStyle(YAML::EmitterStyle::Flow);
    node.push_back(value.x);
    node.push_back(value.y);
    return node;
}

Node make_color(Engine::Color::Color value)
{
    Node node;
    node.SetStyle(YAML::EmitterStyle::Flow);
    node.push_back(value.r);
    node.push_back(value.g);
    node.push_back(value.b);
    node.push_back(value.a);
    return node;
}

} // namespace Yaml
