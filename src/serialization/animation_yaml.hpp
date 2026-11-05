/**
 * @file animation_yaml.hpp
 * @brief YAML serialization for Animation.
 */
#pragma once

#include "gameplay/animation/animation.hpp"
#include "serialization/yaml.hpp"

namespace Yaml {

/**
 * @brief Read an animation from a YAML mapping node.
 * @param node YAML mapping to read from.
 * @param out  Destination animation (must be initialized; frames are appended).
 * @return True when `node` is a mapping.
 */
bool load_animation(const Node* node, Engine::Animation::Animation* out);

/** @brief Build a YAML mapping node describing the animation. */
Node make_animation(const Engine::Animation::Animation* animation);

} // namespace Yaml
