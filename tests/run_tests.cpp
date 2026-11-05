/**
 * @file run_tests.cpp
 * @brief Test suite entry point: registers every test and runs them.
 */
#include "framework.hpp"

/* Test functions, defined in the individual test_*.cpp files. */
void test_types(void);
void test_memory_pool(void);
void test_ecs(void);
void test_camera(void);
void test_resource(void);
void test_image(void);
void test_audio(void);
void test_timers(void);
void test_sprite(void);
void test_yaml(void);

int main(void)
{
    Engine::TestCase::register_test("types",       test_types);
    Engine::TestCase::register_test("memory_pool", test_memory_pool);
    Engine::TestCase::register_test("ecs",         test_ecs);
    Engine::TestCase::register_test("camera",      test_camera);
    Engine::TestCase::register_test("resource",    test_resource);
    Engine::TestCase::register_test("image",       test_image);
    Engine::TestCase::register_test("audio",       test_audio);
    Engine::TestCase::register_test("timers",      test_timers);
    Engine::TestCase::register_test("sprite",      test_sprite);
    Engine::TestCase::register_test("yaml",        test_yaml);
    return Engine::TestCase::run_all();
}
