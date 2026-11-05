/**
 * @file components.cpp
 * @brief Registry of the components the ECS world stores.
 *
 * Keeps the component list (names and sizes) next to the component type
 * definitions in components.hpp, so the world code can stay generic and only
 * this file needs to change when a component is added or removed.
 */
#include "core/ecs/components.hpp"

namespace Engine {
namespace Ecs {

/** One descriptor per ComponentType, in enum order. */
const ComponentInfo COMPONENT_INFO[COMPONENT_COUNT] = {
    { "Transform", sizeof(Transform)        },
    { "Health",    sizeof(HealthComponent)  },
    { "Velocity",  sizeof(VelocityComponent) },
    { "Sprite",    sizeof(Sprite)           },
    { "Hitbox",    sizeof(Hitbox)           },
    { "Hurtbox",   sizeof(Hurtbox)          },
    { "Area",      sizeof(Area)             },
    { "Parent",    sizeof(Parent)           },
};

} // namespace Ecs
} // namespace Engine
