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

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"
#include "yaml-cpp/yaml.h"

#include <string>

namespace Yaml {

/** YAML document node (yaml-cpp's node type). */
typedef YAML::Node Node;

/**
 * @brief Parse a YAML file into `root`.
 * @return True on success; `root` is left untouched on failure.
 */
bool load(Node* root, const char* path);

/**
 * @brief Parse YAML text from memory into `root`.
 * @return True on success; `root` is left untouched on failure.
 */
bool load_text(Node* root, const char* text);

/**
 * @brief Serialize `root` to a YAML file.
 * @return True on success.
 */
bool save(const Node* root, const char* path);

/* Typed accessors with fallback values; never throw. */
i32                  as_int(const Node& node, i32 fallback);
f32                  as_float(const Node& node, f32 fallback);
bool                 as_bool(const Node& node, bool fallback);
std::string          as_string(const Node& node, const char* fallback);
Engine::Vec2::Vec2   as_vec2(const Node& node, Engine::Vec2::Vec2 fallback);
Engine::Color::Color as_color(const Node& node, Engine::Color::Color fallback);

/* Typed accessors without a fallback: write the value and report success.
 * On failure the type's default value (0 / false / empty / zero vector) is
 * written to `out` and false is returned. */
bool get_int(const Node& node, i32* out);
bool get_float(const Node& node, f32* out);
bool get_bool(const Node& node, bool* out);
bool get_string(const Node& node, std::string* out);
bool get_vec2(const Node& node, Engine::Vec2::Vec2* out);
bool get_color(const Node& node, Engine::Color::Color* out);

/** @brief Build a flow-style YAML sequence `[x, y]` from a Vec2. */
Node make_vec2(Engine::Vec2::Vec2 value);

/** @brief Build a flow-style YAML sequence `[r, g, b, a]` from a Color. */
Node make_color(Engine::Color::Color value);

} // namespace Yaml
