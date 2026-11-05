/**
 * @file test_ecs.cpp
 * @brief Tests for the ECS world: entity lifecycle and component storage.
 */
#include "framework.hpp"

#include "core/ecs/world.hpp"
#include "core/ecs/components.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"
#include "core/types/sprite.hpp"

void test_ecs(void)
{
    Engine::Ecs::World world;
    Engine::Ecs::world_init(&world, 4);

    /* Entity creation. */
    Engine::Ecs::Entity e0 = Engine::Ecs::world_create_entity(&world);
    Engine::Ecs::Entity e1 = Engine::Ecs::world_create_entity(&world);
    TEST_CHECK(e0.id == 0 && e0.generation == 1);
    TEST_CHECK(e1.id == 1 && e1.generation == 1);
    TEST_CHECK(Engine::Ecs::world_is_alive(&world, e0));
    TEST_CHECK(world.alive_count == 2);

    /* Add a transform and read it back. */
    Engine::Ecs::Transform t0;
    t0.position = Engine::Vec2::make(1.0f, 2.0f);
    t0.rotation = 0.5f;
    t0.scale    = Engine::Vec2::make(3.0f, 4.0f);
    TEST_CHECK(Engine::Ecs::world_add_component(&world, e0,
                 Engine::Ecs::COMPONENT_TRANSFORM, &t0) != NULL);

    Engine::Ecs::Transform* got = (Engine::Ecs::Transform*)Engine::Ecs::world_get_component(
        &world, e0, Engine::Ecs::COMPONENT_TRANSFORM);
    TEST_CHECK(got != NULL);
    if (got)
    {
        TEST_CHECK_FEQ(got->position.x, 1.0f, 1e-6);
        TEST_CHECK_FEQ(got->position.y, 2.0f, 1e-6);
        TEST_CHECK_FEQ(got->rotation, 0.5f, 1e-6);
    }
    TEST_CHECK(Engine::Ecs::world_count(&world, Engine::Ecs::COMPONENT_TRANSFORM) == 1);

    /* Adding again overwrites in place: count stays the same. */
    Engine::Ecs::Transform t1;
    t1.position = Engine::Vec2::make(9.0f, 9.0f);
    t1.rotation = 0.0f;
    t1.scale    = Engine::Vec2::make(1.0f, 1.0f);
    Engine::Ecs::world_add_component(&world, e0, Engine::Ecs::COMPONENT_TRANSFORM, &t1);
    TEST_CHECK(Engine::Ecs::world_count(&world, Engine::Ecs::COMPONENT_TRANSFORM) == 1);
    got = (Engine::Ecs::Transform*)Engine::Ecs::world_get_component(
        &world, e0, Engine::Ecs::COMPONENT_TRANSFORM);
    TEST_CHECK(got != NULL && got->position.x == 9.0f);

    /* Swap-remove: removing the first of two keeps the second reachable. */
    Engine::Ecs::world_add_component(&world, e1, Engine::Ecs::COMPONENT_TRANSFORM, &t0);
    TEST_CHECK(Engine::Ecs::world_count(&world, Engine::Ecs::COMPONENT_TRANSFORM) == 2);

    Engine::Ecs::world_remove_component(&world, e0, Engine::Ecs::COMPONENT_TRANSFORM);
    TEST_CHECK(Engine::Ecs::world_count(&world, Engine::Ecs::COMPONENT_TRANSFORM) == 1);
    TEST_CHECK(Engine::Ecs::world_get_component(&world, e0,
               Engine::Ecs::COMPONENT_TRANSFORM) == NULL);
    got = (Engine::Ecs::Transform*)Engine::Ecs::world_get_component(
        &world, e1, Engine::Ecs::COMPONENT_TRANSFORM);
    TEST_CHECK(got != NULL);
    if (got)
        TEST_CHECK_FEQ(got->position.x, 1.0f, 1e-6);

    /* Sprite component and dense ordering. */
    Engine::Ecs::Sprite sprite = Engine::Sprite::make(
        7, Engine::Vec2::make(5.0f, 6.0f), 4, 2);
    sprite.color = Engine::Color::make(10.0f, 20.0f, 30.0f, 40.0f);
    Engine::Ecs::world_add_component(&world, e1, Engine::Ecs::COMPONENT_SPRITE, &sprite);
    Engine::Ecs::Sprite* sg = (Engine::Ecs::Sprite*)Engine::Ecs::world_get_component(
        &world, e1, Engine::Ecs::COMPONENT_SPRITE);
    TEST_CHECK(sg != NULL && sg->texture == 7 && sg->color.a == 40);

    Engine::Ecs::Entity owner = Engine::Ecs::world_entity_at(
        &world, Engine::Ecs::COMPONENT_SPRITE, 0);
    TEST_CHECK(owner.id == e1.id);

    /* Destroying an entity frees its components and slot. */
    Engine::Ecs::world_destroy_entity(&world, e1);
    TEST_CHECK(!Engine::Ecs::world_is_alive(&world, e1));
    TEST_CHECK(world.alive_count == 1);
    TEST_CHECK(Engine::Ecs::world_count(&world, Engine::Ecs::COMPONENT_SPRITE) == 0);
    TEST_CHECK(Engine::Ecs::world_count(&world, Engine::Ecs::COMPONENT_TRANSFORM) == 0);
    TEST_CHECK(Engine::Ecs::world_get_component(&world, e1,
               Engine::Ecs::COMPONENT_TRANSFORM) == NULL);

    /* Slot reuse bumps the generation, invalidating the stale handle. */
    Engine::Ecs::Entity e2 = Engine::Ecs::world_create_entity(&world);
    TEST_CHECK(e2.id == e1.id && e2.generation == 3);
    TEST_CHECK(!Engine::Ecs::world_is_alive(&world, e1));
    TEST_CHECK(Engine::Ecs::world_is_alive(&world, e2));

    /* Growing past the initial capacity still works. */
    int alive = 0;
    for (int i = 0; i < 64; ++i)
    {
        Engine::Ecs::Entity e = Engine::Ecs::world_create_entity(&world);
        if (Engine::Ecs::world_is_alive(&world, e))
            ++alive;
    }
    TEST_CHECK(alive == 64);

    Engine::Ecs::world_destroy(&world);
    TEST_CHECK(world.generation == NULL);
    TEST_CHECK(world.free_next == NULL);
    TEST_CHECK(world.capacity == 0);
}
