/**
 * @file render_system.hpp
 * @brief Declares the sprite rendering system.
 */
#pragma once

#include "core/ecs/world.hpp"
#include "graphics/rendering/renderer.hpp"

namespace Engine {
namespace Systems {

/**
 * @brief Draw every entity that has both a Transform and a Sprite.
 */
void render_system_draw(Ecs::World* world, Render::Renderer* renderer);

} // namespace Systems
} // namespace Engine
