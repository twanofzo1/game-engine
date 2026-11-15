/**
 * @file shader_compiler_test.cpp
 * @brief End-to-end tests for the full .shader compilation pipeline.
 *
 * Drives ShaderCompiler::compile (lex -> parse -> transpile) end to end and
 * verifies the produced GLSL plus error reporting across every stage.
 *
 * Build and run with:  scons test
 */
#include "tools/shader_transpiler/shader_compiler.hpp"

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

/** @brief Compile `src` and require success. */
static std::optional<CompiledShader> compile_ok(const char* src)
{
    std::expected<CompiledShader, CompileFailure> r = ShaderCompiler::compile(src);
    if (!r.has_value()) return std::nullopt;
    return *r;
}

/** @brief Compile `src` and return the failure (or nullopt on success). */
static std::optional<CompileFailure> compile_err(const char* src)
{
    std::expected<CompiledShader, CompileFailure> r = ShaderCompiler::compile(src);
    if (r.has_value()) return std::nullopt;
    return r.error();
}

/** @brief True if `haystack` contains `needle`. */
static bool contains(std::string haystack, std::string needle)
{
    return haystack.find(needle) != std::string::npos;
}

/** @brief A minimal but complete .shader source. */
static const char* MINIMAL =
    "Vertex(Vec2 position){ vertex_output(position); }\n"
    "Fragment(Vec2 uv){ fragment_output(Vec4(1.0, 0.0, 0.0, 1.0)); }\n";


/* ----------------------------------------------------------------------- */
/* Success: valid source                                                    */
/* ----------------------------------------------------------------------- */

/** @brief A minimal shader compiles to two non-empty GLSL strings. */
static bool test_minimal_compiles()
{
    std::optional<CompiledShader> r = compile_ok(MINIMAL);
    if (!r.has_value()) return false;
    return !r->vertex_source.empty() && !r->fragment_source.empty();
}

/** @brief The vertex output gets the GLSL version header. */
static bool test_vertex_version()
{
    std::optional<CompiledShader> r = compile_ok(MINIMAL);
    if (!r.has_value()) return false;
    return contains(r->vertex_source, "#version 330 core");
}

/** @brief The fragment output gets the GLSL version header. */
static bool test_fragment_version()
{
    std::optional<CompiledShader> r = compile_ok(MINIMAL);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "#version 330 core");
}

/** @brief The vertex output maps vertex_output to gl_Position. */
static bool test_vertex_output()
{
    std::optional<CompiledShader> r = compile_ok(MINIMAL);
    if (!r.has_value()) return false;
    return contains(r->vertex_source, "gl_Position = uViewProj * vec4(position, 0.0, 1.0);");
}

/** @brief The fragment output maps fragment_output to FragColor. */
static bool test_fragment_output()
{
    std::optional<CompiledShader> r = compile_ok(MINIMAL);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "FragColor = vec4(1.0, 0.0, 0.0, 1.0);");
}

/* ----------------------------------------------------------------------- */
/* Success: uniforms and type mapping                                       */
/* ----------------------------------------------------------------------- */

/** @brief A uniform is declared in both stages with mapped GLSL type. */
static bool test_uniform_both_stages()
{
    const char* src =
        "uniform Vec2 u_resolution;\n"
        "Vertex(Vec2 position){ vertex_output(position); }\n"
        "Fragment(Vec2 uv){ fragment_output(Vec4(1.0)); }\n";
    std::optional<CompiledShader> r = compile_ok(src);
    if (!r.has_value()) return false;
    return contains(r->vertex_source, "uniform vec2 u_resolution;")
        && contains(r->fragment_source, "uniform vec2 u_resolution;");
}

/** @brief A sampler uniform maps to sampler2D. */
static bool test_sampler_uniform()
{
    const char* src =
        "uniform Texture2D u_tex;\n"
        "Vertex(Vec2 p){ vertex_output(p); }\n"
        "Fragment(Vec2 uv){ fragment_output(Vec4(1.0)); }\n";
    std::optional<CompiledShader> r = compile_ok(src);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "uniform sampler2D u_tex;");
}

/* ----------------------------------------------------------------------- */
/* Success: control flow and expressions                                    */
/* ----------------------------------------------------------------------- */

/** @brief An if/else in the fragment stage reaches GLSL. */
static bool test_if_else()
{
    const char* src =
        "Vertex(Vec2 p){ vertex_output(p); }\n"
        "Fragment(Vec2 uv){ Vec4 c = Vec4(1.0); if (uv.x > 0.5) { c = Vec4(0.0); } else { c = Vec4(1.0); } fragment_output(c); }\n";
    std::optional<CompiledShader> r = compile_ok(src);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "if ((uv.x > 0.5))")
        && contains(r->fragment_source, "else");
}

/** @brief A while loop reaches GLSL. */
static bool test_while_loop()
{
    const char* src =
        "Vertex(Vec2 p){ vertex_output(p); }\n"
        "Fragment(Vec2 uv){ float i = 0.0; while (i < 4.0) { i = i + 1.0; } fragment_output(Vec4(i)); }\n";
    std::optional<CompiledShader> r = compile_ok(src);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "while ((i < 4.0))");
}

/** @brief A for loop reaches GLSL with its init statement. */
static bool test_for_loop()
{
    const char* src =
        "Vertex(Vec2 p){ vertex_output(p); }\n"
        "Fragment(Vec2 uv){ for (int i = 0; i < 4; i = i + 1) { } fragment_output(Vec4(1.0)); }\n";
    std::optional<CompiledShader> r = compile_ok(src);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "for (;")
        && contains(r->fragment_source, "int i = 0;");
}

/** @brief A ternary expression reaches GLSL. */
static bool test_ternary()
{
    const char* src =
        "Vertex(Vec2 p){ vertex_output(p); }\n"
        "Fragment(Vec2 uv){ float x = uv.x > 0.5 ? 1.0 : 0.0; fragment_output(Vec4(x)); }\n";
    std::optional<CompiledShader> r = compile_ok(src);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "?");
}

/* ----------------------------------------------------------------------- */
/* Success: typed helper functions                                          */
/* ----------------------------------------------------------------------- */

/** @brief A typed helper function is emitted in the fragment source. */
static bool test_helper_function()
{
    const char* src =
        "Vertex(Vec2 p){ vertex_output(p); }\n"
        "Vec4 mix_red(Vec4 c){ return c; }\n"
        "Fragment(Vec2 uv){ fragment_output(mix_red(Vec4(1.0))); }\n";
    std::optional<CompiledShader> r = compile_ok(src);
    if (!r.has_value()) return false;
    return contains(r->fragment_source, "mix_red");
}

/* ----------------------------------------------------------------------- */
/* Errors: each stage                                                       */
/* ----------------------------------------------------------------------- */

/** @brief Null source produces NoSource. */
static bool test_error_null_source()
{
    std::optional<CompileFailure> f = compile_err(nullptr);
    if (!f.has_value()) return false;
    return f->error == CompileError::NoSource;
}

/** @brief Empty source produces NoSource. */
static bool test_error_empty_source()
{
    std::optional<CompileFailure> f = compile_err("");
    if (!f.has_value()) return false;
    return f->error == CompileError::NoSource;
}

/** @brief An unknown character produces a lex error. */
static bool test_error_lex()
{
    std::optional<CompileFailure> f = compile_err("Vertex(Vec2 p){ $ }");
    if (!f.has_value()) return false;
    return f->error == CompileError::LexError;
}

/** @brief An unterminated block comment produces a lex error. */
static bool test_error_unterminated_comment()
{
    std::optional<CompileFailure> f = compile_err("Vertex(Vec2 p){ /* never closed }");
    if (!f.has_value()) return false;
    return f->error == CompileError::LexError;
}

/** @brief A missing semicolon produces a parse error with a position. */
static bool test_error_parse_position()
{
    std::optional<CompileFailure> f = compile_err("uniform Vec2 u");
    if (!f.has_value()) return false;
    return f->error == CompileError::ParseError
        && f->line >= 1
        && f->column >= 1;
}


/** @brief Run every registered test and report the summary. */
int main()
{
    struct TestCase { const char* name; bool (*fn)(); };

    TestCase tests[] = {
        {"minimal compiles",        test_minimal_compiles},
        {"vertex version",          test_vertex_version},
        {"fragment version",        test_fragment_version},
        {"vertex_output",           test_vertex_output},
        {"fragment_output",         test_fragment_output},
        {"uniform both stages",     test_uniform_both_stages},
        {"sampler uniform",         test_sampler_uniform},
        {"if/else",                 test_if_else},
        {"while loop",              test_while_loop},
        {"for loop",                test_for_loop},
        {"ternary",                 test_ternary},
        {"helper function",         test_helper_function},
        {"null source",             test_error_null_source},
        {"empty source",            test_error_empty_source},
        {"lex error",               test_error_lex},
        {"unterminated comment",    test_error_unterminated_comment},
        {"parse error position",    test_error_parse_position},
    };

    u32 count = sizeof(tests) / sizeof(tests[0]);

    for (u32 i = 0; i < count; i++) {
        test_expect(tests[i].name, tests[i].fn());
    }

    printf("\n%d/%d tests passed\n", g_run - g_failed, g_run);
    return g_failed == 0 ? 0 : 1;
}
