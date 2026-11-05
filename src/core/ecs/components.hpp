/**
 * @file components.hpp
 * @brief Component definitions and the registry of component types the world
 *        can store.
 *
 * Components are pure data structs with no behavior. Generic types (Vec2,
 * Color, Transform2D) live in ../types and are reused here; only the
 * component-specific structures are declared in this file. The ComponentType
 * enum and COMPONENT_INFO table (defined in components.cpp) are the single
 * source of truth for "what lives in the world".
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/ecs/entity.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"
#include "core/types/transform2d.hpp"
#include "core/types/sprite.hpp"
#include "gameplay/collision/collision_shape2d.hpp"

namespace Engine {
namespace Ecs {

struct HealthComponent {
    i32 health;
    i32 max_health;
};

struct VelocityComponent {
    Vec2::Vec2 velocity;
};

/** Max collision shapes one hitbox component can hold (1-to-many). */
#define COMBAT_MAX_SHAPES 4

/** Max subscribers a hitbox/hurtbox/area component can hold. */
#define COMBAT_MAX_LISTENERS 8

/** How a component's listeners fire during a continuous overlap. */
enum TriggerMode {
    TRIGGER_CONSTANT, ///< Fire every frame while overlapping.
    TRIGGER_ON_ENTER  ///< Fire once per overlap; re-arms after leaving.
};

/** Damage-dealing box: one entity with 1-to-many collision shapes. */
struct Hitbox {
    Collision::CollisionShape2D shapes[COMBAT_MAX_SHAPES];
    u32 shape_count;
    Subscriber listeners[COMBAT_MAX_LISTENERS];
    u32 listener_count;
    TriggerMode mode; ///< How listeners fire during an overlap.
    bool active;      ///< Transient: true while currently overlapping.
};

/** Damage-receiving box: one entity with exactly one collision shape. */
struct Hurtbox {
    Collision::CollisionShape2D shape;
    Subscriber listeners[COMBAT_MAX_LISTENERS];
    u32 listener_count;
    TriggerMode mode;
    bool active;
};

/** Trigger zone: one entity with one shape, overlapping only other areas. */
struct Area {
    Collision::CollisionShape2D shape;
    Subscriber listeners[COMBAT_MAX_LISTENERS];
    u32 listener_count;
    TriggerMode mode;
    bool active;
};

/** Parent handle: a child entity inherits its parent's transform. */
struct Parent {
    Entity parent;
};

/** Position, rotation and scale of an entity in world space. */
typedef Transform2D::Transform2D Transform;

/** Visual description of an entity (solid-color or animated sprite). */
typedef Sprite::Sprite Sprite;

/** Registered component types. Add new components here. */
enum ComponentType {
    COMPONENT_TRANSFORM,
    COMPONENT_HEALTH,
    COMPONENT_VELOCITY,
    COMPONENT_SPRITE,
    COMPONENT_HITBOX,
    COMPONENT_HURTBOX,
    COMPONENT_AREA,
    COMPONENT_PARENT,
    COMPONENT_COUNT
};

/** @brief Static description of one registered component type. */
struct ComponentInfo {
    const char* name; ///< Human-readable name, for debugging and inspection.
    u32         size; ///< Size of one component instance in bytes.
};

/**
 * @brief Descriptor table with one entry per registered component type.
 *
 * Entries are in ComponentType order. The world iterates this table so it
 * stays generic, while the component list itself lives in components.cpp.
 */
extern const ComponentInfo COMPONENT_INFO[COMPONENT_COUNT];

} // namespace Ecs
} // namespace Engine
