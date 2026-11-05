/**
 * @file component_pool.hpp
 * @brief Declares the sparse-set component storage used by the ECS world.
 *
 * Each component type owns one pool. Components are stored in dense packed
 * arrays keyed by entity through a sparse map, which gives O(1) add/remove/
 * lookup and cache-friendly iteration over the dense list.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/ecs/entity.hpp"
#include "core/allocator/memory_pool.hpp"

namespace Engine {
namespace Ecs {

namespace ComponentPool {

    /** Sparse-set storage for a single component type. */
struct ComponentPool {
    u32               size;            ///< Component size in bytes (== block size).
    MemoryPool::Pool  blocks;          ///< Pool of component instances.
    u32*              sparse;          ///< Entity index -> dense position + 1 (0 = absent).
    Entity*           dense_entities;  ///< Dense position -> owning entity.
    u32*              dense_blocks;    ///< Dense position -> block index in `blocks`.
    u32               count;           ///< Number of live components.
    u32               dense_capacity;  ///< Allocated dense array length.
    u32               sparse_capacity; ///< Allocated sparse array length.
};

/**
 * @brief Initialize a pool for components of `size` bytes.
 * @param dense_capacity  Initial number of component slots.
 * @param sparse_capacity Initial sparse-map size (>= max entity index + 1).
 */
void init(ComponentPool* pool, u32 size,
                         u32 dense_capacity, u32 sparse_capacity);

/** @brief Release all memory owned by the pool. */
void destroy(ComponentPool* pool);

/** @brief Grow the sparse map to cover `sparse_capacity` entity slots. */
void grow_sparse(ComponentPool* pool, u32 sparse_capacity);

/** @brief Grow the dense arrays to hold `dense_capacity` components. */
void grow_dense(ComponentPool* pool, u32 dense_capacity);

/**
 * @brief Add a component for `entity`.
 * @return Pointer to the component storage (existing storage if already present).
 */
void* add(ComponentPool* pool, Entity entity);

/** @brief Remove the component owned by `entity`, if any. */
void remove(ComponentPool* pool, Entity entity);

/**
 * @brief Look up the component owned by `entity`.
 * @return Pointer to the component, or NULL when absent.
 */
void* get(ComponentPool* pool, Entity entity);

} // namespace ComponentPool
} // namespace Ecs
} // namespace Engine
