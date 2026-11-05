/**
 * @file sprite.hpp
 * @brief YAML serialization for a sprite description.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "serialization/yaml.hpp"

namespace Yaml {

/** Maximum length of a sprite name, including the null terminator. */
#define SPRITE_NAME_MAX 64

/** A sprite as described in YAML. */
struct SpriteConfig {
    char              name[SPRITE_NAME_MAX]; ///< Sprite name ("" = unnamed).
    i32               texture_id; ///< Index into the texture list.
    i32               h_frames;   ///< Horizontal frame count.
    i32               v_frames;   ///< Vertical frame count.
    Engine::Vec2::Vec2 pos;       ///< World position.
    f32               scale;      ///< Uniform scale.
};

/**
 * @brief Read a sprite description from a YAML mapping node.
 * @return True when `node` is a mapping.
 */
bool load_sprite(const Node* node, SpriteConfig* out);

/** @brief Build a YAML mapping node describing the sprite. */
Node make_sprite(const SpriteConfig* sprite);

} // namespace Yaml
