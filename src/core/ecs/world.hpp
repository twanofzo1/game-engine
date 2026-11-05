/**
 * @file world.hpp
 * @brief Declares the ECS world: entity registry and component storage.
 *
 * The world owns a generation-table entity registry and one sparse-set
 * ComponentPool per component type. Systems iterate component pools directly
 * or through the helper functions declared here.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/components.hpp"
#include "core/ecs/component_pool.hpp"

namespace Engine {
namespace Ecs {

/**
 * Entity registry plus one component pool per registered component type.
 * The component list lives in components.hpp/components.cpp (COMPONENT_INFO),
 * so adding a component only touches those files, not the world.
 */
struct World {
    u32          capacity;                       ///< Entity slot capacity.
    u32          alive_count;                    ///< Number of live entities.
    u32          free_head;                      ///< First free slot, or ENTITY_NULL_ID.
    u32*         generation;                     ///< Per-slot generation counter.
    u32*         free_next;                      ///< Per-slot free-list link.
    ComponentPool::ComponentPool pools[COMPONENT_COUNT];        ///< One pool per component type.
};

void world_init(World* world, u32 capacity);
void world_destroy(World* world);

/** @brief Allocate a new entity and return its handle. */
Entity world_create_entity(World* world);

/** @brief Destroy an entity and all of its components. */
void world_destroy_entity(World* world, Entity entity);

/** @brief Return true when the handle refers to a live entity. */
bool world_is_alive(const World* world, Entity entity);

/**
 * @brief Add or overwrite a component for `entity`.
 * @return Pointer to the component storage.
 */
void* world_add_component(World* world, Entity entity, ComponentType type, const void* data);

/**
 * @brief Look up a component of `entity`.
 * @return Pointer to the component, or NULL when absent.
 */
void* world_get_component(World* world, Entity entity, ComponentType type);

/** @brief Remove a component from `entity`, if present. */
void world_remove_component(World* world, Entity entity, ComponentType type);

/** @brief Number of live components of `type`. */
u32 world_count(const World* world, ComponentType type);

/** @brief Entity owning the component at dense position `index`. */
Entity world_entity_at(const World* world, ComponentType type, u32 index);

/** Maximum parent-chain depth resolved by world_get_transform. */
#define ECS_MAX_PARENT_DEPTH 16

/**
 * @brief Compute an entity's world transform by composing its parent chain.
 * @return True when the entity has a Transform component.
 */
bool world_get_transform(World* world, Entity entity, Transform* out);

} // namespace Ecs
} // namespace Engine
