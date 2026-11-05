/**
 * @file resource.hpp
 * @brief Declares a simple file resource cache (load/unload/find by path).
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Resource {

/** Resource kind. */
enum Type {
    RESOURCE_RAW = 0,
    RESOURCE_IMAGE,
    RESOURCE_AUDIO,
    RESOURCE_COUNT
};

/** A cached file loaded into memory. */
struct Resource {
    u32   id;    ///< Unique handle assigned at load time.
    char* path;  ///< File path (owned copy).
    Type  type;  ///< Resource kind.
    u8*   data;  ///< Raw file bytes.
    usize size;  ///< Number of raw bytes.
};

/** A growable cache of loaded resources. */
struct Manager {
    Resource* resources; ///< Dynamic array of cached resources.
    u32       count;     ///< Number of cached resources.
    u32       capacity;  ///< Allocated array length.
};

void init(Manager* manager, u32 capacity);
void destroy(Manager* manager);

/**
 * @brief Load a file into the cache.
 * @return Pointer to the cached resource, or NULL on failure.
 * @note If `path` is already cached, the existing resource is returned.
 * @warning Allocates with malloc; call unload() or destroy() to free it.
 */
Resource* load(Manager* manager, const char* path, Type type);

/** @brief Find a cached resource by path, or NULL. */
Resource* find(const Manager* manager, const char* path);

/** @brief Remove a resource from the cache and free its memory. */
void unload(Manager* manager, const char* path);

/**
 * @brief Read an entire file into a newly allocated buffer.
 * @param out_size Receives the number of bytes read.
 * @return Buffer (caller frees), or NULL on failure.
 * @warning Allocates with malloc.
 */
u8* read_file(const char* path, usize* out_size);

} // namespace Resource
} // namespace Engine
