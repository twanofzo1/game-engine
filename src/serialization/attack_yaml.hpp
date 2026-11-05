/**
 * @file attack_yaml.hpp
 * @brief YAML serialization for Attack.
 */
#pragma once

#include "gameplay/attack/attack.hpp"
#include "serialization/yaml.hpp"

namespace Yaml {

/**
 * @brief Read an attack from a YAML mapping node.
 * @param node YAML mapping to read from.
 * @param out  Destination attack (must be initialized; keyframes are appended).
 * @return True when `node` is a mapping.
 */
bool load_attack(const Node* node, Engine::Attack::Attack* out);

/** @brief Build a YAML mapping node describing the attack. */
Node make_attack(const Engine::Attack::Attack* attack);

} // namespace Yaml
