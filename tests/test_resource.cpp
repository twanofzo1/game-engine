/**
 * @file test_resource.cpp
 * @brief Tests for the file resource cache and raw file reading.
 */
#include "framework.hpp"

#include "resources/resource.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* TEST_PATHS[3] = {
    "__resource_test_0.bin",
    "__resource_test_1.bin",
    "__resource_test_2.bin",
};

static const u8 PAYLOAD[3] = { 'a', 'b', 'c' };

/** @brief Write bytes to `path`; return 0 on failure. */
static int write_file(const char* path, const u8* data, usize size)
{
    FILE* f = fopen(path, "wb");
    if (!f)
        return 0;
    if (size > 0 && fwrite(data, 1, size, f) != size)
    {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

void test_resource(void)
{
    usize size = 0;

    /* read_file: full contents plus a NUL terminator. */
    TEST_CHECK(write_file(TEST_PATHS[0], PAYLOAD, sizeof(PAYLOAD)));

    u8* data = Engine::Resource::read_file(TEST_PATHS[0], &size);
    TEST_CHECK(data != NULL);
    TEST_CHECK(size == sizeof(PAYLOAD));
    TEST_CHECK(memcmp(data, PAYLOAD, sizeof(PAYLOAD)) == 0);
    TEST_CHECK(data[size] == 0);
    free(data);

    /* read_file: a missing file returns NULL. */
    TEST_CHECK(Engine::Resource::read_file("__no_such_file__.bin", &size) == NULL);

    /* Cache: load, dedup, find, unload. */
    Engine::Resource::Manager manager;
    Engine::Resource::init(&manager, 2);

    Engine::Resource::Resource* r1 = Engine::Resource::load(
        &manager, TEST_PATHS[0], Engine::Resource::RESOURCE_RAW);
    TEST_CHECK(r1 != NULL);
    TEST_CHECK(r1->size == sizeof(PAYLOAD));
    TEST_CHECK(manager.count == 1);

    Engine::Resource::Resource* r2 = Engine::Resource::load(
        &manager, TEST_PATHS[0], Engine::Resource::RESOURCE_IMAGE);
    TEST_CHECK(r2 == r1); /* same path -> same resource, first type wins */
    TEST_CHECK(manager.count == 1);
    TEST_CHECK(r2->type == Engine::Resource::RESOURCE_RAW);

    TEST_CHECK(Engine::Resource::find(&manager, TEST_PATHS[0]) == r1);
    TEST_CHECK(Engine::Resource::find(&manager, "other.bin") == NULL);

    Engine::Resource::unload(&manager, TEST_PATHS[0]);
    TEST_CHECK(manager.count == 0);
    TEST_CHECK(Engine::Resource::find(&manager, TEST_PATHS[0]) == NULL);

    /* Loading more files than the initial capacity grows the cache. */
    TEST_CHECK(write_file(TEST_PATHS[0], PAYLOAD, sizeof(PAYLOAD)));
    TEST_CHECK(write_file(TEST_PATHS[1], PAYLOAD, sizeof(PAYLOAD)));
    TEST_CHECK(write_file(TEST_PATHS[2], PAYLOAD, sizeof(PAYLOAD)));

    for (int i = 0; i < 3; ++i)
    {
        Engine::Resource::Resource* r = Engine::Resource::load(
            &manager, TEST_PATHS[i], Engine::Resource::RESOURCE_AUDIO);
        TEST_CHECK(r != NULL);
    }
    TEST_CHECK(manager.count == 3);
    TEST_CHECK(Engine::Resource::find(&manager, TEST_PATHS[1]) != NULL);

    Engine::Resource::destroy(&manager);
    TEST_CHECK(manager.resources == NULL);
    TEST_CHECK(manager.capacity == 0);

    for (int i = 0; i < 3; ++i)
        remove(TEST_PATHS[i]);
}
