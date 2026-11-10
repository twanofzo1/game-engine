/**
 * @file yaml.hpp
 * @brief Thin facade over the vendored yaml-cpp parser/emitter.
 *
 * yaml-cpp (external/yaml-cpp, MIT) is a full-featured YAML parser/emitter
 * that supports block and flow collections. This header re-exposes its node
 * type and adds exception-free `load`/`load_text`/`save` helpers plus typed
 * accessors with fallback values, so callers can read and write YAML without
 * handling exceptions.
 */
#pragma once

#include "yaml-cpp/yaml.h"
#include "core/typedefs.hpp"
#include <string>
#include <optional>

typedef YAML::Node YamlNode;

struct Yaml{
    static std::optional<YamlNode> load(const char* path);
    static std::optional<YamlNode> load_text(const char* text);
    static bool save(const YamlNode* root, const char* path);

    static YamlNode serialise_int(i32 value);
    static YamlNode serialise_float(f32 value);
    static YamlNode serialise_bool(bool value);
    static YamlNode serialise_string(const std::string* value);

    static i32 get_int_or(const YamlNode* node, i32 fallback);
    static f32 get_float_or(const YamlNode* node, f32 fallback);
    static bool get_bool_or(const YamlNode* node, bool fallback);
    static std::string get_string_or(const YamlNode* node, const char* fallback);

    static std::optional<bool> get_bool(const YamlNode* node);
    static std::optional<f32> get_float(const YamlNode* node);
    static std::optional<i32> get_int(const YamlNode* node);
    static std::optional<std::string> get_string(const YamlNode* node);
};



