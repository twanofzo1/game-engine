/**
 * @file resource.cpp
 * @brief Simple file resource cache implementation.
 */
#include "resources/resource.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Resource {

u8* read_file(const char* path, usize* out_size)
{
    FILE* file = fopen(path, "rb");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (length < 0)
    {
        fclose(file);
        return NULL;
    }

    u8* data = (u8*)malloc((size_t)length + 1);
    if (!data)
    {
        fclose(file);
        return NULL;
    }

    if (length > 0 && fread(data, 1, (size_t)length, file) != (size_t)length)
    {
        free(data);
        fclose(file);
        return NULL;
    }

    data[length] = 0;
    fclose(file);
    *out_size = (usize)length;
    return data;
}

/** @brief Grow the cache array to hold at least `needed` resources. */
static void grow(Manager* manager, u32 needed)
{
    if (needed <= manager->capacity)
        return;

    u32 next = manager->capacity == 0 ? 8u : manager->capacity * 2u;
    while (next < needed)
        next *= 2u;

    /* @warning: raw realloc used for the cache array; growth is rare. */
    manager->resources = (Resource*)realloc(manager->resources, sizeof(Resource) * next);
    manager->capacity = next;
}

void init(Manager* manager, u32 capacity)
{
    if (capacity == 0)
        capacity = 8;

    manager->resources = (Resource*)malloc(sizeof(Resource) * capacity);
    manager->count = 0;
    manager->capacity = capacity;
}

void destroy(Manager* manager)
{
    for (u32 i = 0; i < manager->count; ++i)
    {
        free(manager->resources[i].path);
        free(manager->resources[i].data);
    }

    free(manager->resources);
    manager->resources = NULL;
    manager->count = 0;
    manager->capacity = 0;
}

Resource* load(Manager* manager, const char* path, Type type)
{
    Resource* existing = find(manager, path);
    if (existing)
        return existing;

    usize size = 0;
    u8* data = read_file(path, &size);
    if (!data)
        return NULL;

    grow(manager, manager->count + 1);

    Resource* resource = &manager->resources[manager->count];
    resource->id = manager->count;
    resource->path = (char*)malloc(strlen(path) + 1);
    strcpy(resource->path, path);
    resource->type = type;
    resource->data = data;
    resource->size = size;
    ++manager->count;
    return resource;
}

Resource* find(const Manager* manager, const char* path)
{
    for (u32 i = 0; i < manager->count; ++i)
    {
        if (strcmp(manager->resources[i].path, path) == 0)
            return (Resource*)&manager->resources[i];
    }
    return NULL;
}

void unload(Manager* manager, const char* path)
{
    for (u32 i = 0; i < manager->count; ++i)
    {
        if (strcmp(manager->resources[i].path, path) != 0)
            continue;

        free(manager->resources[i].path);
        free(manager->resources[i].data);
        manager->resources[i] = manager->resources[manager->count - 1];
        --manager->count;
        return;
    }
}

} // namespace Resource
} // namespace Engine
