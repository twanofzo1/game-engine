/**
 * @file parser_stmt.cpp
 * @brief Statement dispatch, declarations and expression statements.
 */
#include "core/frontend/shader_parsing/parser/parser_internal.hpp"


/** @brief Push a statement node and return its index. */
u32 parser_push_stmt(ParserState* ps, StmtKind kind, u32 a, u32 b, u32 c, u32 d)
{
    Stmt s;
    s.kind = kind;
    s.a = a;
    s.b = b;
    s.c = c;
    s.d = d;
    ps->shader.stmts.push_back(s);
    return static_cast<u32>(ps->shader.stmts.size()) - 1;
}

/**
 * @brief True if the current position starts a local variable declaration.
 *
 * The pattern `Type Identifier` followed by `=` or `;` is treated as a
 * declaration (rather than an expression statement).
 */
static bool parser_looks_like_decl(ParserState* ps)
{
    const Token* t0 = parser_current(ps);
    const Token* t1 = parser_peek(ps, 1);
    if ((t0->kind == TokenKind::TypeKeyword || t0->kind == TokenKind::Identifier)
        && t1->kind == TokenKind::Identifier) {
        TokenKind t2 = parser_peek(ps, 2)->kind;
        return t2 == TokenKind::Assign || t2 == TokenKind::Semicolon;
    }
    return false;
}

/**
 * @brief Parse a local variable declaration `Type name [= expr];`.
 *
 * The declaration is stored in Shader::var_names/var_types; the statement's
 * `a` field indexes into those vectors and `b` holds the initialiser
 * expression index (0 if none).
 */
static std::optional<u32> parser_parse_var_decl(ParserState* ps)
{
    std::string type = parser_current(ps)->lexeme;
    parser_advance(ps);
    std::string name = parser_current(ps)->lexeme;
    parser_advance(ps);

    u32 initializer = 0;
    if (parser_match(ps, TokenKind::Assign)) {
        std::optional<u32> init = parser_parse_expression(ps);
        if (!init.has_value()) {
            return std::nullopt;
        }
        initializer = *init;
    }
    if (!parser_expect(ps, TokenKind::Semicolon, "';'")) {
        return std::nullopt;
    }

    ps->shader.var_names.push_back(name);
    ps->shader.var_types.push_back(type);
    u32 var_slot = static_cast<u32>(ps->shader.var_names.size()) - 1;

    return parser_push_stmt(ps, StmtKind::Variable, var_slot, initializer, 0, 0);
}

/** @brief Parse an expression statement `expr;`. */
static std::optional<u32> parser_parse_expr_stmt(ParserState* ps)
{
    std::optional<u32> e = parser_parse_expression(ps);
    if (!e.has_value()) {
        return std::nullopt;
    }
    if (!parser_expect(ps, TokenKind::Semicolon, "';'")) {
        return std::nullopt;
    }
    return parser_push_stmt(ps, StmtKind::Expression, *e, 0, 0, 0);
}

/**
 * @brief Parse a statement.
 */
std::optional<u32> parser_parse_statement(ParserState* ps)
{
    // Block.
    if (parser_check(ps, TokenKind::LeftBrace)) {
        return parser_parse_block(ps);
    }
    // Empty statement.
    if (parser_match(ps, TokenKind::Semicolon)) {
        return parser_push_stmt(ps, StmtKind::Empty, 0, 0, 0, 0);
    }
    // Control flow.
    if (parser_check_word(ps, "return")) {
        return parser_parse_return(ps);
    }
    if (parser_check_word(ps, "if")) {
        return parser_parse_if(ps);
    }
    if (parser_check_word(ps, "for")) {
        return parser_parse_for(ps);
    }
    if (parser_check_word(ps, "while")) {
        return parser_parse_while(ps);
    }
    // Local variable declaration.
    if (parser_looks_like_decl(ps)) {
        std::optional<u32> decl = parser_parse_var_decl(ps);
        if (decl.has_value()) {
            return decl;
        }
        return std::nullopt;
    }
    // Expression statement.
    return parser_parse_expr_stmt(ps);
}
