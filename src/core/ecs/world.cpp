/**
 * @file world.cpp
 * @brief Implementation of the ECS world: entity lifecycle and component access.
 */
#include "core/ecs/world.hpp"

#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Ecs {

static const u32 FREE_END = ENTITY_NULL_ID;

void world_init(World* world, u32 capacity)
{
    world->capacity    = capacity;
    world->alive_count = 0;
    world->free_head   = 0;
    world->generation  = (u32*)calloc(capacity, sizeof(u32));
    world->free_next   = (u32*)malloc(sizeof(u32) * capacity);

    for (u32 i = 0; i < capacity; ++i)
        world->free_next[i] = i + 1;
    world->free_next[capacity - 1] = FREE_END;

    for (int t = 0; t < COMPONENT_COUNT; ++t)
        ComponentPool::init(&world->pools[t], COMPONENT_INFO[t].size,
                            capacity, capacity);
}

void world_destroy(World* world)
{
    for (int t = 0; t < COMPONENT_COUNT; ++t)
        ComponentPool::destroy(&world->pools[t]);

    free(world->generation);
    free(world->free_next);
    world->generation  = NULL;
    world->free_next   = NULL;
    world->capacity    = 0;
    world->alive_count = 0;
    world->free_head   = FREE_END;
}

/**
 * @brief Double the entity slot capacity and grow every sparse map.
 * @warning Only valid when the free list is empty (free_head == FREE_END).
 */
static void world_grow(World* world)
{
    u32 old  = world->capacity;
    u32 next = old * 2;

    /* @warning: raw realloc used for the registry arrays; growth is rare. */
    world->generation = (u32*)realloc(world->generation, sizeof(u32) * next);
    memset(world->generation + old, 0, sizeof(u32) * (next - old));

    world->free_next = (u32*)realloc(world->free_next, sizeof(u32) * next);
    for (u32 i = old; i < next; ++i)
        world->free_next[i] = i + 1;
    world->free_next[next - 1] = FREE_END;

    world->free_head = old;

    for (int t = 0; t < COMPONENT_COUNT; ++t)
        ComponentPool::grow_sparse(&world->pools[t], next);

    world->capacity = next;
}

Entity world_create_entity(World* world)
{
    if (world->free_head == FREE_END)
        world_grow(world);

    u32 index = world->free_head;
    world->free_head = world->free_next[index];

    /* @warning: generation is 32 bits; it wraps only after 2^32
     *            incarnations of a single slot, which is unreachable. */
    ++world->generation[index];
    ++world->alive_count;

    return entity_make(index, world->generation[index]);
}

void world_destroy_entity(World* world, Entity entity)
{
    if (!world_is_alive(world, entity))
        return;

    u32 index = entity.id;

    for (int t = 0; t < COMPONENT_COUNT; ++t)
        ComponentPool::remove(&world->pools[t], entity);

    /* Bump the generation so any outstanding handle to this slot goes stale
     * immediately, even before the slot is reused. */
    ++world->generation[index];

    world->free_next[index] = world->free_head;
    world->free_head = index;
    --world->alive_count;
}

bool world_is_alive(const World* world, Entity entity)
{
    /* ENTITY_NULL_ID is >= capacity, so an unset handle fails this check. */
    return entity.id < world->capacity
        && entity.generation != 0
        && world->generation[entity.id] == entity.generation;
}

void* world_add_component(World* world, Entity entity, ComponentType type, const void* data)
{
    if (!world_is_alive(world, entity))
        return NULL;

    void* dest = ComponentPool::add(&world->pools[type], entity);
    memcpy(dest, data, world->pools[type].size);
    return dest;
}

void* world_get_component(World* world, Entity entity, ComponentType type)
{
    if (!world_is_alive(world, entity))
        return NULL;
    return ComponentPool::get(&world->pools[type], entity);
}

void world_remove_component(World* world, Entity entity, ComponentType type)
{
    if (!world_is_alive(world, entity))
        return;
    ComponentPool::remove(&world->pools[type], entity);
}

u32 world_count(const World* world, ComponentType type)
{
    return world->pools[type].count;
}

Entity world_entity_at(const World* world, ComponentType type, u32 index)
{
    return world->pools[type].dense_entities[index];
}

bool world_get_transform(World* world, Entity entity, Transform* out)
{
    Transform chain[ECS_MAX_PARENT_DEPTH];
    u32 depth = 0;
    Entity e   = entity;

    /* Collect transforms from the leaf up to the root. */
    while (depth < ECS_MAX_PARENT_DEPTH)
    {
        Transform* t = (Transform*)world_get_component(world, e, COMPONENT_TRANSFORM);
        if (!t)
            return false;
        chain[depth++] = *t;

        Parent* p = (Parent*)world_get_component(world, e, COMPONENT_PARENT);
        if (!p || !world_is_alive(world, p->parent))
            break;
        e = p->parent;
    }

    /* Compose from the root down to the leaf. */
    Transform result = chain[depth - 1];
    for (u32 i = depth - 1; i > 0; --i)
        result = Transform2D::compose(result, chain[i - 1]);

    *out = result;
    return true;
}

} // namespace Ecs
} // namespace Engine
