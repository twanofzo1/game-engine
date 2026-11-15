/**
 * @file parser_stmt_control.cpp
 * @brief Parsing of block and control-flow statements.
 */
#include "tools/shader_parser/parser/parser_internal.hpp"


/**
 * @brief Parse a block statement `{ ... }`.
 *
 * Children are appended, in source order, to Shader::block_children; the
 * block node records the start index in `a` and the count in `b`.
 */
std::optional<u32> parser_parse_block(ParserState* ps)
{
    if (!parser_expect(ps, TokenKind::LeftBrace, "'{'")) {
        return std::nullopt;
    }

    // Collect child statement indices locally: nested blocks append to the
    // shared block_children vector while they parse, so deferring our own
    // append until after the loop keeps this block's children contiguous.
    std::vector<u32> local_children;

    while (!parser_check(ps, TokenKind::RightBrace)) {
        if (parser_check(ps, TokenKind::EndOfFile)) {
            ps->failed_token = parser_current(ps);
            ps->failed_message = "unterminated block (missing '}')";
            return std::nullopt;
        }
        std::optional<u32> child = parser_parse_statement(ps);
        if (!child.has_value()) {
            return std::nullopt;
        }
        local_children.push_back(*child);
    }
    parser_advance(ps); // '}'

    u32 child_start = static_cast<u32>(ps->shader.block_children.size());
    for (u32 i = 0; i < local_children.size(); i++) {
        ps->shader.block_children.push_back(local_children[i]);
    }
    u32 child_count = static_cast<u32>(local_children.size());
    return parser_push_stmt(ps, StmtKind::Block, child_start, child_count, 0, 0);
}

/** @brief Parse a `return [expr];` statement. */
std::optional<u32> parser_parse_return(ParserState* ps)
{
    parser_advance(ps); // 'return'

    u32 value = 0;
    if (!parser_check(ps, TokenKind::Semicolon)) {
        std::optional<u32> e = parser_parse_expression(ps);
        if (!e.has_value()) {
            return std::nullopt;
        }
        value = *e;
    }
    if (!parser_expect(ps, TokenKind::Semicolon, "';'")) {
        return std::nullopt;
    }
    return parser_push_stmt(ps, StmtKind::Return, value, 0, 0, 0);
}

/** @brief Parse an `if / else if / else` statement. */
std::optional<u32> parser_parse_if(ParserState* ps)
{
    parser_advance(ps); // 'if'

    if (!parser_expect(ps, TokenKind::LeftParen, "'('")) {
        return std::nullopt;
    }
    std::optional<u32> cond = parser_parse_expression(ps);
    if (!cond.has_value()) {
        return std::nullopt;
    }
    if (!parser_expect(ps, TokenKind::RightParen, "')'")) {
        return std::nullopt;
    }

    std::optional<u32> then_stmt = parser_parse_statement(ps);
    if (!then_stmt.has_value()) {
        return std::nullopt;
    }

    u32 else_stmt = 0;
    if (parser_check_word(ps, "else")) {
        parser_advance(ps);
        if (parser_check_word(ps, "if")) {
            std::optional<u32> elif = parser_parse_if(ps);
            if (!elif.has_value()) {
                return std::nullopt;
            }
            else_stmt = *elif;
        } else {
            std::optional<u32> e = parser_parse_statement(ps);
            if (!e.has_value()) {
                return std::nullopt;
            }
            else_stmt = *e;
        }
    }

    // a = condition, b = then, c = else (0 if none).
    return parser_push_stmt(ps, StmtKind::If, *cond, *then_stmt, else_stmt, 0);
}

/** @brief Parse a `for (...)` loop. */
std::optional<u32> parser_parse_for(ParserState* ps)
{
    parser_advance(ps); // 'for'

    if (!parser_expect(ps, TokenKind::LeftParen, "'('")) {
        return std::nullopt;
    }

    // Initialiser (declaration or expression or empty).
    u32 init = parser_push_stmt(ps, StmtKind::Empty, 0, 0, 0, 0);
    if (parser_check(ps, TokenKind::Semicolon)) {
        parser_advance(ps); // ';'
    } else {
        std::optional<u32> i = parser_parse_statement(ps);
        if (!i.has_value()) {
            return std::nullopt;
        }
        init = *i;
    }

    // Condition (optional).
    u32 cond = 0;
    if (!parser_check(ps, TokenKind::Semicolon)) {
        std::optional<u32> c = parser_parse_expression(ps);
        if (!c.has_value()) {
            return std::nullopt;
        }
        cond = *c;
    }
    parser_advance(ps); // ';'

    // Step (optional).
    u32 step = 0;
    if (!parser_check(ps, TokenKind::RightParen)) {
        std::optional<u32> s = parser_parse_expression(ps);
        if (!s.has_value()) {
            return std::nullopt;
        }
        step = *s;
    }
    if (!parser_expect(ps, TokenKind::RightParen, "')'")) {
        return std::nullopt;
    }

    std::optional<u32> body = parser_parse_statement(ps);
    if (!body.has_value()) {
        return std::nullopt;
    }

    // a = init, b = cond, c = step, d = body.
    return parser_push_stmt(ps, StmtKind::For, init, cond, step, *body);
}

/** @brief Parse a `while (...)` loop. */
std::optional<u32> parser_parse_while(ParserState* ps)
{
    parser_advance(ps); // 'while'

    if (!parser_expect(ps, TokenKind::LeftParen, "'('")) {
        return std::nullopt;
    }
    std::optional<u32> cond = parser_parse_expression(ps);
    if (!cond.has_value()) {
        return std::nullopt;
    }
    if (!parser_expect(ps, TokenKind::RightParen, "')'")) {
        return std::nullopt;
    }
    std::optional<u32> body = parser_parse_statement(ps);
    if (!body.has_value()) {
        return std::nullopt;
    }
    // a = cond, b = body.
    return parser_push_stmt(ps, StmtKind::While, *cond, *body, 0, 0);
}
