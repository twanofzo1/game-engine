/**
 * @file framework.cpp
 * @brief Test harness implementation: registry, runner and check accounting.
 */
#include "framework.hpp"

#include <stdio.h>

namespace Engine {
namespace TestCase {

#define TEST_MAX_CASES 256

static TestCase g_cases[TEST_MAX_CASES];
static int      g_count    = 0;
static int      g_checks   = 0;
static int      g_failures = 0;

void register_test(const char* name, void (*fn)(void))
{
    if (g_count >= TEST_MAX_CASES)
        return;
    g_cases[g_count].name = name;
    g_cases[g_count].fn   = fn;
    ++g_count;
}

void check(int ok, const char* file, int line, const char* expr, const char* msg)
{
    ++g_checks;
    if (ok)
        return;
    ++g_failures;
    fprintf(stderr, "  FAIL %s:%d  %s  %s\n", file, line, expr, msg);
}

void check_feq(f64 a, f64 b, f64 eps, const char* file, int line, const char* expr)
{
    ++g_checks;
    if (a - b >= -eps && a - b <= eps)
        return;
    ++g_failures;
    fprintf(stderr, "  FAIL %s:%d  %s  (%.6f vs %.6f, eps %.6f)\n",
            file, line, expr, a, b, eps);
}

int run_all(void)
{
    int i;
    for (i = 0; i < g_count; ++i)
    {
        int before = g_failures;
        printf("[ RUN  ] %s\n", g_cases[i].name);
        g_cases[i].fn();
        printf("[ %s ] %s\n", g_failures == before ? " OK " : "FAIL", g_cases[i].name);
    }

    printf("\n%d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}

} // namespace TestCase
} // namespace Engine
