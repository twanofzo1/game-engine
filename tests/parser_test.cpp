/**
 * @file parser_test.cpp
 * @brief Tests for the shader-language parser.
 *
 * A minimal, dependency-free test harness mirroring lexer_test.cpp. Each
 * test returns a boolean; failures print the test name and are counted.
 *
 * Build and run with:  scons test
 */
#include "tools/shader_parser/parser/parser.hpp"
#include "tools/shader_parser/lexer/lexer.hpp"

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
 * @param name  Human-readable test name.
 * @param cond  Result of the test body.
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

/**
 * @brief Tokenise + parse `src`, returning the resulting AST.
 * @return The Shader on success, or nullopt (errors ignored) on failure.
 */
static std::optional<Shader> parse_ok(const char* src)
{
    std::expected<std::vector<Token>, LexerError> lexed = Lexer::tokenise({.source = src});
    if (!lexed.has_value()) {
        return std::nullopt;
    }
    std::expected<Shader, ParseFailure> parsed = Parser::parse({.tokens = &*lexed});
    if (!parsed.has_value()) {
        return std::nullopt;
    }
    return *parsed;
}

/** @brief Parse `src`, returning the ParseFailure (or nullopt on success). */
static std::optional<ParseFailure> parse_err(const char* src)
{
    std::expected<std::vector<Token>, LexerError> lexed = Lexer::tokenise({.source = src});
    if (!lexed.has_value()) {
        return std::nullopt;
    }
    std::expected<Shader, ParseFailure> parsed = Parser::parse({.tokens = &*lexed});
    if (parsed.has_value()) {
        return std::nullopt;
    }
    return parsed.error();
}


/* ----------------------------------------------------------------------- */
/* Includes and uniforms                                                    */
/* ----------------------------------------------------------------------- */

/** @brief A `#include` directive produces one Include declaration. */
static bool test_include()
{
    std::optional<Shader> s = parse_ok("#include \"other.shader\"");
    if (!s.has_value()) return false;
    if (s->decls.size() != 1) return false;
    return s->decls[0].kind == DeclKind::Include
        && s->decls[0].name == "\"other.shader\"";
}

/** @brief A uniform declaration produces one Uniform declaration. */
static bool test_uniform()
{
    std::optional<Shader> s = parse_ok("uniform Vec2 u_resolution;");
    if (!s.has_value()) return false;
    if (s->decls.size() != 1) return false;
    return s->decls[0].kind == DeclKind::Uniform
        && s->decls[0].type == "Vec2"
        && s->decls[0].name == "u_resolution";
}

/* ----------------------------------------------------------------------- */
/* Functions                                                                */
/* ----------------------------------------------------------------------- */

/** @brief A `Vertex(...)` entry point is recognised with the Vertex stage. */
static bool test_vertex_function()
{
    std::optional<Shader> s = parse_ok(
        "Vertex(Vec2 position){ vertex_output(position); }");
    if (!s.has_value()) return false;
    if (s->fn_stages.size() != 1) return false;
    return s->fn_stages[0] == Stage::Vertex
        && s->fn_names[0] == "Vertex"
        && s->fn_param_count[0] == 1
        && s->params[0].type == "Vec2"
        && s->params[0].name == "position";
}

/** @brief A `Fragment(...)` entry point is recognised with the Fragment stage. */
static bool test_fragment_function()
{
    std::optional<Shader> s = parse_ok("Fragment(Vec2 uv){}");
    if (!s.has_value()) return false;
    if (s->fn_stages.size() != 1) return false;
    return s->fn_stages[0] == Stage::Fragment
        && s->fn_names[0] == "Fragment";
}

/**
 * @brief A fragment with a body that assigns a color and calls
 * `fragment_output` parses to the expected statement tree shape.
 */
static bool test_fragment_body()
{
    std::optional<Shader> s = parse_ok(
        "Fragment(Vec2 uv){ Vec4 color2 = Vec4(1.0, 0.0, 0.0, 1.0); fragment_output(color2); }");
    if (!s.has_value()) return false;

    // The body is a block with two children.
    u32 body = s->fn_body[0];
    if (body >= s->stmts.size()) return false;
    const Stmt* block = &s->stmts[body];
    if (block->kind != StmtKind::Block) return false;
    return block->b == 2; // two statements.
}

/** @brief A typed helper function is parsed with its explicit return type. */
static bool test_typed_helper()
{
    std::optional<Shader> s = parse_ok(
        "float add(float a, float b){ return a + b; }");
    if (!s.has_value()) return false;
    if (s->fn_names.size() != 1) return false;
    return s->fn_names[0] == "add"
        && s->fn_types[0] == "float"
        && s->fn_stages[0] == Stage::None
        && s->fn_param_count[0] == 2;
}

/* ----------------------------------------------------------------------- */
/* Expressions                                                              */
/* ----------------------------------------------------------------------- */

/** @brief A constructor-style call with four arguments parses. */
static bool test_constructor_call()
{
    std::optional<Shader> s = parse_ok(
        "Fragment(Vec2 uv){ Vec4 c = Vec4(1.0, 0.0, 0.0, 1.0); }");
    if (!s.has_value()) return false;

    // Find the call expression.
    for (const Expr* e = &s->exprs[0]; e < &s->exprs[0] + s->exprs.size(); ++e) {
        if (e->kind == ExprKind::Call && e->text == "4") {
            return true;
        }
    }
    return false;
}

/** @brief Arithmetic and comparison operators combine at the right precedence. */
static bool test_binary_precedence()
{
    std::optional<Shader> s = parse_ok(
        "float f(){ return 1.0f + 2.0f * 3.0f; }");
    if (!s.has_value()) return false;

    // The top-level expression is a Plus whose rhs is a Star (since `*`
    // binds tighter than `+`).
    for (const Expr* e = &s->exprs[0]; e < &s->exprs[0] + s->exprs.size(); ++e) {
        if (e->kind == ExprKind::Binary && e->text == "+") {
            const Expr* rhs = &s->exprs[e->b];
            return rhs->kind == ExprKind::Binary && rhs->text == "*";
        }
    }
    return false;
}

/** @brief Unary minus applies correctly. */
static bool test_unary_minus()
{
    std::optional<Shader> s = parse_ok(
        "float f(){ return -x; }");
    if (!s.has_value()) return false;
    for (const Expr* e = &s->exprs[0]; e < &s->exprs[0] + s->exprs.size(); ++e) {
        if (e->kind == ExprKind::Unary && e->text == "-") {
            return true;
        }
    }
    return false;
}

/** @brief Member access is parsed as a Member node. */
static bool test_member_access()
{
    std::optional<Shader> s = parse_ok(
        "Fragment(Vec2 uv){ Vec2 v = uv.xy; }");
    if (!s.has_value()) return false;
    for (const Expr* e = &s->exprs[0]; e < &s->exprs[0] + s->exprs.size(); ++e) {
        if (e->kind == ExprKind::Member && e->text == "xy") {
            return true;
        }
    }
    return false;
}

/** @brief A ternary expression parses. */
static bool test_ternary()
{
    std::optional<Shader> s = parse_ok(
        "float f(bool c){ return c ? 1.0f : 2.0f; }");
    if (!s.has_value()) return false;
    for (const Expr* e = &s->exprs[0]; e < &s->exprs[0] + s->exprs.size(); ++e) {
        if (e->kind == ExprKind::Ternary) {
            return true;
        }
    }
    return false;
}

/* ----------------------------------------------------------------------- */
/* Control flow                                                             */
/* ----------------------------------------------------------------------- */

/** @brief An if/else statement parses with a non-zero else branch. */
static bool test_if_else()
{
    std::optional<Shader> s = parse_ok(
        "float f(bool c){ if (c) { return 1.0f; } else { return 2.0f; } }");
    if (!s.has_value()) return false;
    for (const Stmt* st = &s->stmts[0]; st < &s->stmts[0] + s->stmts.size(); ++st) {
        if (st->kind == StmtKind::If) {
            return st->c != 0; // has an else branch.
        }
    }
    return false;
}

/** @brief A for loop parses with init, cond, step and body. */
static bool test_for_loop()
{
    std::optional<Shader> s = parse_ok(
        "float f(){ for (int i = 0; i < 10; i = i + 1) { i = i; } }");
    if (!s.has_value()) return false;
    for (const Stmt* st = &s->stmts[0]; st < &s->stmts[0] + s->stmts.size(); ++st) {
        if (st->kind == StmtKind::For) {
            return st->a != 0 && st->b != 0 && st->c != 0 && st->d != 0;
        }
    }
    return false;
}

/** @brief A while loop parses with cond and body. */
static bool test_while_loop()
{
    std::optional<Shader> s = parse_ok(
        "float f(bool c){ while (c) { c = false; } }");
    if (!s.has_value()) return false;
    for (const Stmt* st = &s->stmts[0]; st < &s->stmts[0] + s->stmts.size(); ++st) {
        if (st->kind == StmtKind::While) {
            return st->a != 0 && st->b != 0;
        }
    }
    return false;
}

/** @brief A return statement parses with a value. */
static bool test_return_value()
{
    std::optional<Shader> s = parse_ok("float f(){ return 42; }");
    if (!s.has_value()) return false;
    for (const Stmt* st = &s->stmts[0]; st < &s->stmts[0] + s->stmts.size(); ++st) {
        if (st->kind == StmtKind::Return) {
            return st->a != 0; // has a value.
        }
    }
    return false;
}

/* ----------------------------------------------------------------------- */
/* Errors                                                                   */
/* ----------------------------------------------------------------------- */

/** @brief A missing semicolon after a uniform produces an error. */
static bool test_error_missing_semicolon()
{
    std::optional<ParseFailure> f = parse_err("uniform Vec2 u");
    if (!f.has_value()) return false;
    return f->error == ParserError::UnexpectedToken;
}

/** @brief An unterminated block produces an error. */
static bool test_error_unterminated_block()
{
    std::optional<ParseFailure> f = parse_err("Vertex(Vec2 p){");
    if (!f.has_value()) return false;
    return f->error == ParserError::UnexpectedToken;
}

/** @brief A stray token at top level produces an error. */
static bool test_error_stray_token()
{
    std::optional<ParseFailure> f = parse_err(";");
    if (!f.has_value()) return false;
    return f->error == ParserError::UnexpectedToken;
}

/* ----------------------------------------------------------------------- */
/* Integration                                                              */
/* ----------------------------------------------------------------------- */

/** @brief The user's example shader parses to the expected declaration count. */
static bool test_example_shader()
{
    const char* src =
        "#include \"other_shader.shader\"\n"
        "uniform Vec2 u_resolution;\n"
        "\n"
        "Vertex(Vec2 position){\n"
        "    vertex_output(position);\n"
        "}\n"
        "\n"
        "Fragment(Vec2 uv){\n"
        "    Vec4 color2 = Vec4(1.0, 0.0, 0.0, 1.0);\n"
        "    fragment_output(color2);\n"
        "}\n";
    std::optional<Shader> s = parse_ok(src);
    if (!s.has_value()) return false;

    u32 includes = 0, uniforms = 0;
    for (const Decl* d = &s->decls[0]; d < &s->decls[0] + s->decls.size(); ++d) {
        if (d->kind == DeclKind::Include) includes++;
        if (d->kind == DeclKind::Uniform) uniforms++;
    }
    return includes == 1 && uniforms == 1
        && s->fn_names.size() == 2
        && s->fn_stages[0] == Stage::Vertex
        && s->fn_stages[1] == Stage::Fragment;
}


/** @brief Run every registered test and report the summary. */
int main()
{
    struct TestCase { const char* name; bool (*fn)(); };

    TestCase tests[] = {
        {"include directive",          test_include},
        {"uniform declaration",        test_uniform},
        {"vertex function",            test_vertex_function},
        {"fragment function",          test_fragment_function},
        {"fragment body",              test_fragment_body},
        {"typed helper function",      test_typed_helper},
        {"constructor call",           test_constructor_call},
        {"binary precedence",          test_binary_precedence},
        {"unary minus",                test_unary_minus},
        {"member access",              test_member_access},
        {"ternary",                    test_ternary},
        {"if/else",                    test_if_else},
        {"for loop",                   test_for_loop},
        {"while loop",                 test_while_loop},
        {"return value",               test_return_value},
        {"error missing semicolon",    test_error_missing_semicolon},
        {"error unterminated block",   test_error_unterminated_block},
        {"error stray token",          test_error_stray_token},
        {"example shader",             test_example_shader},
    };

    u32 count = sizeof(tests) / sizeof(tests[0]);

    for (u32 i = 0; i < count; i++) {
        test_expect(tests[i].name, tests[i].fn());
    }

    printf("\n%d/%d tests passed\n", g_run - g_failed, g_run);
    return g_failed == 0 ? 0 : 1;
}
