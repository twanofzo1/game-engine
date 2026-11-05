/**
 * @file entity.hpp
 * @brief Entity handle type.
 *
 * An entity is a pair of 32-bit values: an id that indexes the world's
 * registry slot, and a generation counter used to detect stale (destroyed)
 * handles. Keeping the two fields separate avoids bit packing and keeps the
 * handle a plain two-field struct.
 */
#pragma once

#include "core/typedefs.hpp"

/** Sentinel id used for "no entity". Macros cannot be namespaced. */
#define ENTITY_NULL_ID 0xFFFFFFFFu

namespace Engine {
namespace Ecs {

/** Entity handle: registry slot id plus a generation counter. */
struct Entity {
    u32 id;         ///< Slot index in the world registry.
    u32 generation; ///< Generation counter for stale-handle detection.
};

/**
 * @brief Build an entity handle from a slot id and a generation counter.
 */
static inline Entity entity_make(u32 id, u32 generation)
{
    Entity entity;
    entity.id = id;
    entity.generation = generation;
    return entity;
}

} // namespace Ecs
} // namespace Engine
