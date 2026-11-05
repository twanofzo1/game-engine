/**
 * @file collision_shape2d_yaml.hpp
 * @brief YAML serialization for CollisionShape2D.
 */
#pragma once

#include "gameplay/collision/collision_shape2d.hpp"
#include "serialization/yaml.hpp"

namespace Yaml {

/**
 * @brief Read a collision shape from a YAML mapping node.
 * @param node YAML mapping to read from.
 * @param out  Destination shape (must be initialized).
 * @return True when `node` is a mapping.
 */
bool load_collision_shape(const Node* node, Engine::Collision::CollisionShape2D* out);

/** @brief Build a YAML mapping node describing the collision shape. */
Node make_collision_shape(const Engine::Collision::CollisionShape2D* shape);

} // namespace Yaml
