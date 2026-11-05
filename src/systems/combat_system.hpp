/**
 * @file combat_system.hpp
 * @brief Declares the combat/trigger system (hitbox-hurtbox and area overlap).
 */
#pragma once

#include "core/ecs/world.hpp"

namespace Engine {
namespace Systems {

/** One hitbox-vs-hurtbox overlap (payload for hit listeners). */
struct Hit {
    Ecs::Entity attacker; ///< Entity owning the hitbox.
    Ecs::Entity defender; ///< Entity owning the hurtbox.
};

/** One area-vs-area overlap (payload for overlap listeners). */
struct Overlap {
    Ecs::Entity owner_a; ///< First area's entity.
    Ecs::Entity owner_b; ///< Second area's entity.
};

/**
 * @brief Transform a local-space shape to world space using an entity transform.
 * @warning Scale is applied to positions but not to a shape's radius/size.
 */
void shape_to_world(const Collision::CollisionShape2D* local,
                    const Ecs::Transform* transform,
                    Collision::CollisionShape2D* out);

/**
 * @brief Resolve every hitbox/hurtbox overlap and every area/area overlap,
 *        notifying each involved component's listeners.
 */
void combat_system_update(Ecs::World* world);

} // namespace Systems
} // namespace Engine
