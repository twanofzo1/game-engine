/**
 * @file texture.hpp
 * @brief YAML serialization for a texture asset description.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "serialization/yaml.hpp"

/** Maximum length of a texture file path, including the null terminator. */
#define TEXTURE_PATH_MAX 256

namespace Yaml {

/** A texture asset as described in YAML. */
struct TextureConfig {
    char              path[TEXTURE_PATH_MAX]; ///< Image file path.
    i32               filter; ///< 0 = nearest, 1 = linear.
    i32               repeat; ///< 0 = clamp, 1 = repeat.
    Engine::Vec2::Vec2 size;  ///< World-space display size (0 = pixel size).
};

/**
 * @brief Read a texture description from a YAML mapping node.
 * @return True when `node` is a mapping.
 */
bool load_texture(const Node* node, TextureConfig* out);

/** @brief Build a YAML mapping node describing the texture. */
Node make_texture(const TextureConfig* texture);

} // namespace Yaml
