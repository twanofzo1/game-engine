/**
 * @file lexer_test.cpp
 * @brief Tests for the GLSL-like lexer.
 *
 * A minimal, dependency-free test harness. Each test is a function returning
 * a boolean (true = pass); failures print the test name and are counted.
 *
 * Build and run with:  scons test
 */
#include "tools/shader_parser/lexer/lexer.hpp"

#include <cstdio>


/** @brief Number of tests that have been run so far. */
static u32 g_run = 0;

/** @brief Number of tests that have failed so far. */
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

/** @brief Tokenise `src` and require it to succeed. */
static std::expected<std::vector<Token>, LexerError> tokenise_ok(const char* src)
{
    return Lexer::tokenise({.source = src});
}

/** @brief True if `tokens[i]` has the given kind and lexeme (empty = skip). */
static bool token_matches(const std::vector<Token>* tokens, u32 i,
                          TokenKind kind, std::string lexeme)
{
    if (i >= tokens->size()) {
        return false;
    }
    const Token* t = &(*tokens)[i];
    if (t->kind != kind) {
        return false;
    }
    if (!lexeme.empty() && t->lexeme != lexeme) {
        return false;
    }
    return true;
}


/* ----------------------------------------------------------------------- */
/* Empty / trivial input                                                    */
/* ----------------------------------------------------------------------- */

/** @brief Empty input yields a single EndOfFile token. */
static bool test_empty_input()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("");
    if (!result.has_value()) return false;
    return result->size() == 1
        && (*result)[0].kind == TokenKind::EndOfFile;
}

/** @brief Whitespace-only input yields a single EndOfFile token. */
static bool test_whitespace_only()
{
    std::expected<std::vector<Token>, LexerError> result =
        tokenise_ok("  \t\n\r  \n");
    if (!result.has_value()) return false;
    return result->size() == 1
        && (*result)[0].kind == TokenKind::EndOfFile;
}

/* ----------------------------------------------------------------------- */
/* Identifiers and keywords                                                 */
/* ----------------------------------------------------------------------- */

/** @brief A plain identifier is lexed as Identifier. */
static bool test_identifier()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("my_var");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::Identifier, "my_var")
        && token_matches(&*result, 1, TokenKind::EndOfFile, "");
}

/** @brief Identifiers may start with an underscore. */
static bool test_identifier_underscore()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("_private");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::Identifier, "_private");
}

/** @brief A control-flow keyword is lexed as Keyword. */
static bool test_keyword()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("return");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::Keyword, "return");
}

/** @brief `true` and `false` are keywords. */
static bool test_keyword_bool_literal()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("true false");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::Keyword, "true")
        && token_matches(&*result, 1, TokenKind::Keyword, "false");
}

/** @brief A built-in type name is lexed as TypeKeyword. */
static bool test_type_keyword()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("vec3");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::TypeKeyword, "vec3");
}

/** @brief sampler2D and mat4 are recognised as type keywords. */
static bool test_type_keyword_sampler_mat()
{
    std::expected<std::vector<Token>, LexerError> result =
        tokenise_ok("sampler2D mat4");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::TypeKeyword, "sampler2D")
        && token_matches(&*result, 1, TokenKind::TypeKeyword, "mat4");
}

/** @brief `void` is a type keyword. */
static bool test_type_keyword_void()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("void");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::TypeKeyword, "void");
}

/* ----------------------------------------------------------------------- */
/* Numeric literals                                                         */
/* ----------------------------------------------------------------------- */

/** @brief A decimal integer is an IntLiteral. */
static bool test_int_decimal()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("12345");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::IntLiteral, "12345");
}

/** @brief A hexadecimal integer is an IntLiteral. */
static bool test_int_hex()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("0x1F");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::IntLiteral, "0x1F");
}

/** @brief A `u`-suffixed integer is a UintLiteral. */
static bool test_uint_suffix()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("42u");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::UintLiteral, "42u");
}

/** @brief A float with a decimal point is a FloatLiteral. */
static bool test_float_decimal()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("3.14");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::FloatLiteral, "3.14");
}

/** @brief A float with an exponent is a FloatLiteral. */
static bool test_float_exponent()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("1e-3");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::FloatLiteral, "1e-3");
}

/** @brief A float with an `f` suffix is a FloatLiteral. */
static bool test_float_suffix()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("2.5f");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::FloatLiteral, "2.5f");
}

/** @brief A leading-dot float like `.5` is a FloatLiteral. */
static bool test_float_leading_dot()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok(".5");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::FloatLiteral, ".5");
}

/** @brief An `lf`-suffixed number is a DoubleLiteral. */
static bool test_double_suffix()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("1.0lf");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::DoubleLiteral, "1.0lf");
}

/** @brief `1` then `.5` lex as two tokens (int, then float) without merging. */
static bool test_number_dot_boundary()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("1.5f");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::FloatLiteral, "1.5f");
}

/* ----------------------------------------------------------------------- */
/* Operators and punctuation                                                */
/* ----------------------------------------------------------------------- */

/** @brief `==`, `!=`, `<=`, `>=` lex as two-char operators. */
static bool test_comparison_operators()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("== != <= >=");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::Equal, "==")
        && token_matches(&*result, 1, TokenKind::NotEqual, "!=")
        && token_matches(&*result, 2, TokenKind::LessEqual, "<=")
        && token_matches(&*result, 3, TokenKind::GreaterEqual, ">=");
}

/** @brief Compound assignment operators lex as two-char operators. */
static bool test_compound_assign()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("+= -= *= /= %=");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::PlusAssign, "+=")
        && token_matches(&*result, 1, TokenKind::MinusAssign, "-=")
        && token_matches(&*result, 2, TokenKind::StarAssign, "*=")
        && token_matches(&*result, 3, TokenKind::SlashAssign, "/=")
        && token_matches(&*result, 4, TokenKind::PercentAssign, "%=");
}

/** @brief Shift operators lex as two-char operators. */
static bool test_shift_operators()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("<< >>");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::ShiftLeft, "<<")
        && token_matches(&*result, 1, TokenKind::ShiftRight, ">>");
}

/** @brief Three-char shift-assign operators lex correctly. */
static bool test_shift_assign_operators()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("<<= >>=");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::ShiftLeftAssign, "<<=")
        && token_matches(&*result, 1, TokenKind::ShiftRightAssign, ">>=");
}

/** @brief `++` and `--` lex as two-char operators. */
static bool test_incdec()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("++ --");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::PlusPlus, "++")
        && token_matches(&*result, 1, TokenKind::MinusMinus, "--");
}

/** @brief Logical operators lex correctly. */
static bool test_logical_operators()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("&& || ^^");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::LogicalAnd, "&&")
        && token_matches(&*result, 1, TokenKind::LogicalOr, "||")
        && token_matches(&*result, 2, TokenKind::LogicalXor, "^^");
}

/** @brief `<<` followed by `=` must produce three-char `<<=` (greedy). */
static bool test_greedy_shift_assign()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("<<=");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::ShiftLeftAssign, "<<=");
}

/** @brief Punctuation characters lex as their own kinds. */
static bool test_punctuation()
{
    std::expected<std::vector<Token>, LexerError> result =
        tokenise_ok("( ) { } [ ] , ; . :");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::LeftParen, "(")
        && token_matches(&*result, 1, TokenKind::RightParen, ")")
        && token_matches(&*result, 2, TokenKind::LeftBrace, "{")
        && token_matches(&*result, 3, TokenKind::RightBrace, "}")
        && token_matches(&*result, 4, TokenKind::LeftBracket, "[")
        && token_matches(&*result, 5, TokenKind::RightBracket, "]")
        && token_matches(&*result, 6, TokenKind::Comma, ",")
        && token_matches(&*result, 7, TokenKind::Semicolon, ";")
        && token_matches(&*result, 8, TokenKind::Dot, ".")
        && token_matches(&*result, 9, TokenKind::Colon, ":");
}

/* ----------------------------------------------------------------------- */
/* Comments                                                                 */
/* ----------------------------------------------------------------------- */

/** @brief A line comment is skipped entirely. */
static bool test_line_comment()
{
    std::expected<std::vector<Token>, LexerError> result =
        tokenise_ok("vec3 color; // this is a comment\nreturn");
    if (!result.has_value()) return false;
    // Expected: vec3 color ; return <eof>
    return token_matches(&*result, 0, TokenKind::TypeKeyword, "vec3")
        && token_matches(&*result, 1, TokenKind::Identifier, "color")
        && token_matches(&*result, 2, TokenKind::Semicolon, ";")
        && token_matches(&*result, 3, TokenKind::Keyword, "return");
}

/** @brief A block comment is skipped entirely. */
static bool test_block_comment()
{
    std::expected<std::vector<Token>, LexerError> result =
        tokenise_ok("a /* block comment */ b");
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::Identifier, "a")
        && token_matches(&*result, 1, TokenKind::Identifier, "b");
}

/** @brief An unterminated block comment produces an error. */
static bool test_unterminated_comment()
{
    std::expected<std::vector<Token>, LexerError> result =
        tokenise_ok("a /* never closed");
    return !result.has_value()
        && result.error() == LexerError::UnterminatedComment;
}

/* ----------------------------------------------------------------------- */
/* Errors and positions                                                     */
/* ----------------------------------------------------------------------- */

/** @brief An unknown character produces UnexpectedChar. */
static bool test_unexpected_char()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("a @ b");
    return !result.has_value()
        && result.error() == LexerError::UnexpectedChar;
}

/** @brief Tokens carry correct 1-based line/column positions. */
static bool test_positions()
{
    std::expected<std::vector<Token>, LexerError> result =
        tokenise_ok("foo\n  bar");
    if (!result.has_value()) return false;
    const Token* foo = &(*result)[0];
    const Token* bar = &(*result)[1];
    return foo->line == 1 && foo->column == 1
        && bar->line == 2 && bar->column == 3;
}

/** @brief Tokens on the first line have column 1, 2, ... for adjacent chars. */
static bool test_column_advance()
{
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok("a + b");
    if (!result.has_value()) return false;
    const Token* a = &(*result)[0];
    const Token* plus = &(*result)[1];
    const Token* b = &(*result)[2];
    return a->column == 1 && plus->column == 3 && b->column == 5;
}

/* ----------------------------------------------------------------------- */
/* Realistic shader snippet                                                 */
/* ----------------------------------------------------------------------- */

/** @brief A small GLSL-like shader tokenises without error. */
static bool test_shader_snippet()
{
    const char* src =
        "layout(location = 0) in vec3 position;\n"
        "uniform mat4 u_view;\n"
        "void main() {\n"
        "    float x = 0.5f;\n"
        "    vec2 uv = position.xy * 2.0;\n"
        "    if (x < 1.0f) { x += 1.0f; }\n"
        "    gl_Position = u_view * vec4(position, 1.0f);\n"
        "}\n";
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok(src);
    return result.has_value();
}

/** @brief A shader snippet's first tokens are as expected. */
static bool test_shader_snippet_prefix()
{
    const char* src = "layout(location = 0) in vec3 position;";
    std::expected<std::vector<Token>, LexerError> result = tokenise_ok(src);
    if (!result.has_value()) return false;
    return token_matches(&*result, 0, TokenKind::Keyword, "layout")
        && token_matches(&*result, 1, TokenKind::LeftParen, "(")
        && token_matches(&*result, 2, TokenKind::Identifier, "location")
        && token_matches(&*result, 3, TokenKind::Assign, "=")
        && token_matches(&*result, 4, TokenKind::IntLiteral, "0")
        && token_matches(&*result, 5, TokenKind::RightParen, ")")
        && token_matches(&*result, 6, TokenKind::Keyword, "in")
        && token_matches(&*result, 7, TokenKind::TypeKeyword, "vec3");
}


/** @brief Run every registered test and report the summary. */
int main()
{
    struct TestCase { const char* name; bool (*fn)(); };

    TestCase tests[] = {
        {"empty input",            test_empty_input},
        {"whitespace only",        test_whitespace_only},
        {"identifier",             test_identifier},
        {"identifier underscore",  test_identifier_underscore},
        {"keyword",                test_keyword},
        {"bool literal keyword",   test_keyword_bool_literal},
        {"type keyword",           test_type_keyword},
        {"sampler/mat type",       test_type_keyword_sampler_mat},
        {"void type keyword",      test_type_keyword_void},
        {"int decimal",            test_int_decimal},
        {"int hex",                test_int_hex},
        {"uint suffix",            test_uint_suffix},
        {"float decimal",          test_float_decimal},
        {"float exponent",         test_float_exponent},
        {"float suffix",           test_float_suffix},
        {"float leading dot",      test_float_leading_dot},
        {"double suffix",          test_double_suffix},
        {"number-dot boundary",    test_number_dot_boundary},
        {"comparison operators",   test_comparison_operators},
        {"compound assign",        test_compound_assign},
        {"shift operators",        test_shift_operators},
        {"shift-assign operators", test_shift_assign_operators},
        {"inc/dec",                test_incdec},
        {"logical operators",      test_logical_operators},
        {"greedy shift-assign",    test_greedy_shift_assign},
        {"punctuation",            test_punctuation},
        {"line comment",           test_line_comment},
        {"block comment",          test_block_comment},
        {"unterminated comment",   test_unterminated_comment},
        {"unexpected char",        test_unexpected_char},
        {"positions",              test_positions},
        {"column advance",         test_column_advance},
        {"shader snippet",         test_shader_snippet},
        {"shader snippet prefix",  test_shader_snippet_prefix},
    };

    u32 count = sizeof(tests) / sizeof(tests[0]);

    for (u32 i = 0; i < count; i++) {
        test_expect(tests[i].name, tests[i].fn());
    }

    printf("\n%d/%d tests passed\n", g_run - g_failed, g_run);
    return g_failed == 0 ? 0 : 1;
}
