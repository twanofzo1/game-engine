/**
 * @file parser_expr_operand.cpp
 * @brief Parsing of primary and postfix expressions.
 */
#include "tools/shader_transpiler/parser/parser_internal.hpp"


/**
 * @brief Parse a primary expression (literal, name, parenthesised).
 */
std::optional<u32> parser_parse_primary(ParserState* ps)
{
    const Token* t = parser_current(ps);

    if (t->kind == TokenKind::IntLiteral) {
        parser_advance(ps);
        return parser_push_expr(ps, ExprKind::Int, t->lexeme, 0, 0, 0);
    }
    if (t->kind == TokenKind::UintLiteral) {
        parser_advance(ps);
        return parser_push_expr(ps, ExprKind::Int, t->lexeme, 0, 0, 0);
    }
    if (t->kind == TokenKind::FloatLiteral
        || t->kind == TokenKind::DoubleLiteral) {
        parser_advance(ps);
        return parser_push_expr(ps, ExprKind::Float, t->lexeme, 0, 0, 0);
    }
    if (t->kind == TokenKind::Keyword
        && (t->lexeme == "true" || t->lexeme == "false")) {
        parser_advance(ps);
        return parser_push_expr(ps, ExprKind::Bool, t->lexeme, 0, 0, 0);
    }
    if (t->kind == TokenKind::Identifier) {
        parser_advance(ps);
        return parser_push_expr(ps, ExprKind::Variable, t->lexeme, 0, 0, 0);
    }
    if (parser_match(ps, TokenKind::LeftParen)) {
        std::optional<u32> inner = parser_parse_expression(ps);
        if (!inner.has_value()) {
            ps->failed_token = t;
            ps->failed_message = "expected an expression inside parentheses";
            return std::nullopt;
        }
        if (!parser_expect(ps, TokenKind::RightParen, "')'")) {
            return std::nullopt;
        }
        return inner;
    }

    ps->failed_token = t;
    ps->failed_message = "expected an expression";
    return std::nullopt;
}

/**
 * @brief Parse a postfix expression (calls and member access).
 */
std::optional<u32> parser_parse_postfix(ParserState* ps)
{
    std::optional<u32> node = parser_parse_primary(ps);
    if (!node.has_value()) {
        return std::nullopt;
    }

    for (;;) {
        // Function / constructor call.
        if (parser_check(ps, TokenKind::LeftParen)) {
            parser_advance(ps);
            // Collect argument root indices locally first: nested calls push
            // their own arguments into expr_args, so appending ours only after
            // parsing keeps each call's arguments contiguous.
            std::vector<u32> local_args;
            if (!parser_check(ps, TokenKind::RightParen)) {
                std::optional<u32> a = parser_parse_expression(ps);
                if (!a.has_value()) {
                    return std::nullopt;
                }
                local_args.push_back(*a);
                while (parser_match(ps, TokenKind::Comma)) {
                    std::optional<u32> b = parser_parse_expression(ps);
                    if (!b.has_value()) {
                        return std::nullopt;
                    }
                    local_args.push_back(*b);
                }
            }
            if (!parser_expect(ps, TokenKind::RightParen, "')'")) {
                return std::nullopt;
            }
            u32 arg_start = static_cast<u32>(ps->shader.expr_args.size());
            for (u32 i = 0; i < local_args.size(); i++) {
                ps->shader.expr_args.push_back(local_args[i]);
            }
            u32 arg_count = static_cast<u32>(local_args.size());
            // Callee in `a`, argument start in `b`, count in `text`.
            node = parser_push_expr(ps, ExprKind::Call, std::to_string(arg_count),
                                    *node, arg_start, 0);
            continue;
        }
        // Member access.
        if (parser_match(ps, TokenKind::Dot)) {
            if (!parser_check(ps, TokenKind::Identifier)) {
                return std::nullopt;
            }
            std::string member = parser_current(ps)->lexeme;
            parser_advance(ps);
            // Reuse the callee's variable name by storing the member as text.
            node = parser_push_expr(ps, ExprKind::Member, member, *node, 0, 0);
            continue;
        }
        break;
    }
    return node;
}
