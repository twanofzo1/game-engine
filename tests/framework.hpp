/**
 * @file framework.hpp
 * @brief Minimal test harness: registration, assertions and a runner.
 *
 * Kept dependency-free (stdio only) so the test suite links against the pure
 * engine logic without pulling in GLFW, GL or a window.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace TestCase {

/** One registered test: a name and the function that runs it. */
struct TestCase {
    const char* name;  ///< Human-readable test name.
    void (*fn)(void);  ///< Test function.
};

/** @brief Register a test function under `name`. */
void register_test(const char* name, void (*fn)(void));

/** @brief Run every registered test; returns the number of failures. */
int run_all(void);

/** @brief Record a boolean check (used by TEST_CHECK). */
void check(int ok, const char* file, int line, const char* expr, const char* msg);

/** @brief Record a float equality check (used by TEST_CHECK_FEQ). */
void check_feq(f64 a, f64 b, f64 eps, const char* file, int line, const char* expr);

} // namespace TestCase
} // namespace Engine

/** @brief Assert `EXPR` is true; records and continues on failure. */
#define TEST_CHECK(EXPR) \
    Engine::TestCase::check((EXPR) != 0, __FILE__, __LINE__, #EXPR, "")

/** @brief Assert `EXPR` is true, with an extra message on failure. */
#define TEST_CHECK_MSG(EXPR, MSG) \
    Engine::TestCase::check((EXPR) != 0, __FILE__, __LINE__, #EXPR, MSG)

/** @brief Assert `a` and `b` are within `eps` of each other. */
#define TEST_CHECK_FEQ(A, B, EPS) \
    Engine::TestCase::check_feq((f64)(A), (f64)(B), (f64)(EPS), \
                                __FILE__, __LINE__, #A " ~= " #B)
