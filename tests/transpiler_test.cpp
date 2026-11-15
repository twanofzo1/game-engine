/**
 * @file transpiler_test.cpp
 * @brief Tests for the shader-language transpiler.
 *
 * Drives the full pipeline (lex -> parse -> transpile) and checks the
 * generated GLSL for expected fragments. A minimal, dependency-free harness
 * in the same style as lexer_test.cpp and parser_test.cpp.
 *
 * Build and run with:  scons test
 */
#include "tools/shader_transpiler/transpiler/transpiler.hpp"
#include "tools/shader_transpiler/parser/parser.hpp"
#include "tools/shader_transpiler/lexer/lexer.hpp"

#include <cstdio>
#include <optional>


/** @brief Number of tests run so far (this binary only). */
static u32 g_run = 0;

/** @brief Number of failed tests so far (this binary only). */
static u32 g_failed = 0;

/** @brief Colours used when printing pass/fail markers. */
#define TEST_PASS "\033[32mPASS\033[0m"
#define TEST_FAIL "\033[31mFAIL\033[0m"


/**
 * @brief Run a single test and record its result.
 */
static void test_expect(const char* name, bool cond)
{
    g_run++;
    if (cond) {
        printf("  [" TEST_PASS "] %s\n", name);
    } else {
        g_failed++;
        printf("  [" TEST_FAIL "] %s\n", name);
    }
}

/** @brief Lex + parse + transpile `src`, returning the result. */
static std::optional<TranspileResult> transpile_ok(const char* src)
{
    std::expected<std::vector<Token>, LexerError> lexed = Lexer::tokenise({.source = src});
    if (!lexed.has_value()) return std::nullopt;
    std::expected<Shader, ParseFailure> parsed = Parser::parse({.tokens = &*lexed});
    if (!parsed.has_value()) return std::nullopt;
    std::expected<TranspileResult, TranspileError> t = Transpiler::transpile(&*parsed);
    if (!t.has_value()) return std::nullopt;
    return *t;
}

/** @brief True if `haystack` contains `needle`. */
static bool contains(std::string haystack, std::string needle)
{
    return haystack.find(needle) != std::string::npos;
}


/* ----------------------------------------------------------------------- */
/* Headers and defaults                                                     */
/* ----------------------------------------------------------------------- */

/** @brief The vertex stage always starts with the GLSL version line. */
static bool test_vertex_version()
{
    std::optional<TranspileResult> r = transpile_ok("Vertex(Vec2 position){} Fragment(Vec2 uv){}");
    if (!r.has_value()) return false;
    return contains(r->vertex_source, "#version 330 core");
}

/** @brief The vertex stage declares the position attribute and projection. */
static bool test_vertex_defaults()
{
    std::optional<TranspileResult> r = transpile_ok("Vertex(Vec2 position){} Fragment(Vec2 uv){}");
    if (!r.has_value()) return false;
    return contains(r->vertex_source, "layout(location = 0) in vec3 aPos;")
        && contains(r->vertex_source, "uniform mat4 uViewProj;");
}

/** @brief The fragment stage declares the texture sampler and output. */
static bool test_fragment_defaults()
{
    std::optional<TranspileResult> r = transpile_ok("Vertex(Vec2 position){} Fragment(Vec2 uv){}");
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "uniform sampler2D uTexture;")
        && contains(r->fragment_source, "out vec4 FragColor;");
}

/* ----------------------------------------------------------------------- */
/* Type mapping                                                             */
/* ----------------------------------------------------------------------- */

/** @brief `Vec2` uniforms map to `vec2`. */
static bool test_type_mapping_uniform()
{
    std::optional<TranspileResult> r = transpile_ok(
        "uniform Vec2 u_resolution; Vertex(Vec2 position){} Fragment(Vec2 uv){}");
    if (!r.has_value()) return false;
    return contains(r->vertex_source, "uniform vec2 u_resolution;")
        && contains(r->fragment_source, "uniform vec2 u_resolution;");
}

/* ----------------------------------------------------------------------- */
/* Entry-point body emission                                                */
/* ----------------------------------------------------------------------- */

/** @brief `vertex_output` maps to `gl_Position` assignment. */
static bool test_vertex_output()
{
    std::optional<TranspileResult> r = transpile_ok(
        "Vertex(Vec2 position){ vertex_output(position); } Fragment(Vec2 uv){ fragment_output(Vec4(1.0,0.0,0.0,1.0)); }");
    if (!r.has_value()) return false;
    return contains(r->vertex_source, "gl_Position = uViewProj * vec4(position, 0.0, 1.0);");
}

/** @brief `fragment_output` maps to `FragColor` assignment. */
static bool test_fragment_output()
{
    std::optional<TranspileResult> r = transpile_ok(
        "Vertex(Vec2 position){} Fragment(Vec2 uv){ fragment_output(Vec4(1.0,0.0,0.0,1.0)); }");
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "FragColor = vec4(1.0, 0.0, 0.0, 1.0);");
}

/** @brief A constructor call with four float args emits as `vec4(...)`. */
static bool test_vec4_constructor()
{
    std::optional<TranspileResult> r = transpile_ok(
        "Vertex(Vec2 p){} Fragment(Vec2 uv){ Vec4 c = Vec4(1.0, 0.0, 0.0, 1.0); fragment_output(c); }");
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "vec4 c = vec4(1.0, 0.0, 0.0, 1.0);");
}

/* ----------------------------------------------------------------------- */
/* Expressions and control flow                                             */
/* ----------------------------------------------------------------------- */

/** @brief Arithmetic expressions emit with parentheses and correct ops. */
static bool test_binary_expression()
{
    std::optional<TranspileResult> r = transpile_ok(
        "Vertex(Vec2 p){} Fragment(Vec2 uv){ float x = 1.0 + 2.0 * 3.0; fragment_output(Vec4(x)); }");
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "float x = (1.0 + (2.0 * 3.0));");
}

/** @brief An if/else statement emits GLSL `if`/`else`. */
static bool test_if_else()
{
    std::optional<TranspileResult> r = transpile_ok(
        "Vertex(Vec2 p){} Fragment(Vec2 uv){ Vec4 c = Vec4(1.0); if (uv.x > 0.5) { c = Vec4(0.0); } else { c = Vec4(1.0); } fragment_output(c); }");
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "if ((uv.x > 0.5))")
        && contains(r->fragment_source, "else");
}

/** @brief A for loop emits its init then a `for (; cond; step)` form. */
static bool test_for_loop()
{
    std::optional<TranspileResult> r = transpile_ok(
        "Vertex(Vec2 p){} Fragment(Vec2 uv){ for (int i = 0; i < 4; i = i + 1) { } fragment_output(Vec4(1.0)); }");
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "for (;")
        && contains(r->fragment_source, "int i = 0;");
}

/** @brief Member access emits with a dot. */
static bool test_member_access()
{
    std::optional<TranspileResult> r = transpile_ok(
        "Vertex(Vec2 p){} Fragment(Vec2 uv){ fragment_output(Vec4(uv.x, uv.y, 0.0, 1.0)); }");
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "uv.x");
}

/* ----------------------------------------------------------------------- */
/* End-to-end example                                                       */
/* ----------------------------------------------------------------------- */

/** @brief The full user example produces coherent output for both stages. */
static bool test_example_shader()
{
    const char* src =
        "#include \"other_shader.shader\"\n"
        "uniform Vec2 u_resolution;\n"
        "Vertex(Vec2 position){\n"
        "    vertex_output(position);\n"
        "}\n"
        "Fragment(Vec2 uv){\n"
        "    Vec4 color2 = Vec4(1.0, 0.0, 0.0, 1.0);\n"
        "    fragment_output(color2);\n"
        "}\n";
    std::optional<TranspileResult> r = transpile_ok(src);
    if (!r.has_value()) return false;

    bool vs_ok = contains(r->vertex_source, "uniform vec2 u_resolution;")
        && contains(r->vertex_source, "gl_Position = uViewProj * vec4(position, 0.0, 1.0);");
    bool fs_ok = contains(r->fragment_source, "uniform vec2 u_resolution;")
        && contains(r->fragment_source, "vec4 color2 = vec4(1.0, 0.0, 0.0, 1.0);")
        && contains(r->fragment_source, "FragColor = color2;");
    return vs_ok && fs_ok;
}


/** @brief Run every registered test and report the summary. */
int main()
{
    struct TestCase { const char* name; bool (*fn)(); };

    TestCase tests[] = {
        {"vertex version",        test_vertex_version},
        {"vertex defaults",       test_vertex_defaults},
        {"fragment defaults",     test_fragment_defaults},
        {"type mapping uniform",  test_type_mapping_uniform},
        {"vertex_output",         test_vertex_output},
        {"fragment_output",       test_fragment_output},
        {"vec4 constructor",      test_vec4_constructor},
        {"binary expression",     test_binary_expression},
        {"if/else",               test_if_else},
        {"for loop",              test_for_loop},
        {"member access",         test_member_access},
        {"example shader",        test_example_shader},
    };

    u32 count = sizeof(tests) / sizeof(tests[0]);

    for (u32 i = 0; i < count; i++) {
        test_expect(tests[i].name, tests[i].fn());
    }

    printf("\n%d/%d tests passed\n", g_run - g_failed, g_run);
    return g_failed == 0 ? 0 : 1;
}
