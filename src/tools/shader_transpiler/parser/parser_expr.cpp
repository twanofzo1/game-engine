/**
 * @file parser_expr.cpp
 * @brief Expression entry points (assignment, ternary) and operator tables.
 */
#include "tools/shader_transpiler/parser/parser_internal.hpp"


/** @brief Precedence level of binary operators (higher binds tighter). */
static u32 parser_binary_precedence(TokenKind kind)
{
    switch (kind) {
    case TokenKind::LogicalOr:  return 1;
    case TokenKind::LogicalXor: return 2;
    case TokenKind::LogicalAnd: return 3;
    case TokenKind::Pipe:       return 4;
    case TokenKind::Caret:      return 5;
    case TokenKind::Ampersand:  return 6;
    case TokenKind::Equal:
    case TokenKind::NotEqual:   return 7;
    case TokenKind::Less:
    case TokenKind::LessEqual:
    case TokenKind::Greater:
    case TokenKind::GreaterEqual: return 8;
    case TokenKind::ShiftLeft:
    case TokenKind::ShiftRight: return 9;
    case TokenKind::Plus:
    case TokenKind::Minus:      return 10;
    case TokenKind::Star:
    case TokenKind::Slash:
    case TokenKind::Percent:    return 11;
    default: return 0;
    }
}

/** @brief True if `kind` is a binary operator. */
static bool parser_is_binary(TokenKind kind)
{
    return parser_binary_precedence(kind) != 0;
}

/** @brief True if `kind` is an assignment operator. */
static bool parser_is_assign(TokenKind kind)
{
    switch (kind) {
    case TokenKind::Assign:
    case TokenKind::PlusAssign:
    case TokenKind::MinusAssign:
    case TokenKind::StarAssign:
    case TokenKind::SlashAssign:
    case TokenKind::PercentAssign:
    case TokenKind::ShiftLeftAssign:
    case TokenKind::ShiftRightAssign:
    case TokenKind::AndAssign:
    case TokenKind::OrAssign:
    case TokenKind::XorAssign:
        return true;
    default:
        return false;
    }
}

/** @brief Push an expression node and return its index. */
u32 parser_push_expr(ParserState* ps, ExprKind kind, std::string text,
                     u32 a, u32 b, u32 c)
{
    Expr e;
    e.kind = kind;
    e.text = text;
    e.a = a;
    e.b = b;
    e.c = c;
    ps->shader.exprs.push_back(e);
    return static_cast<u32>(ps->shader.exprs.size()) - 1;
}

/**
 * @brief Parse a unary expression (`-`, `+`, `!`, `~` prefixes).
 */
static std::optional<u32> parser_parse_unary(ParserState* ps)
{
    TokenKind k = parser_current(ps)->kind;
    if (k == TokenKind::Minus || k == TokenKind::Plus
        || k == TokenKind::Bang || k == TokenKind::Tilde) {
        std::string op = parser_current(ps)->lexeme;
        parser_advance(ps);
        std::optional<u32> operand = parser_parse_unary(ps);
        if (!operand.has_value()) {
            return std::nullopt;
        }
        return parser_push_expr(ps, ExprKind::Unary, op, *operand, 0, 0);
    }
    return parser_parse_postfix(ps);
}

/**
 * @brief Parse a binary expression at or above `min_prec`.
 */
static std::optional<u32> parser_parse_binary(ParserState* ps, u32 min_prec)
{
    std::optional<u32> lhs = parser_parse_unary(ps);
    if (!lhs.has_value()) {
        return std::nullopt;
    }

    for (;;) {
        TokenKind k = parser_current(ps)->kind;
        u32 prec = parser_binary_precedence(k);
        if (prec == 0 || prec < min_prec) {
            break;
        }
        std::string op = parser_current(ps)->lexeme;
        parser_advance(ps);

        std::optional<u32> rhs = parser_parse_binary(ps, prec + 1);
        if (!rhs.has_value()) {
            return std::nullopt;
        }
        lhs = parser_push_expr(ps, ExprKind::Binary, op, *lhs, *rhs, 0);
    }
    return lhs;
}

/**
 * @brief Parse an assignment expression, including compound assignment.
 */
std::optional<u32> parser_parse_assign(ParserState* ps, u32 lhs)
{
    if (!parser_is_assign(parser_current(ps)->kind)) {
        return lhs;
    }
    std::string op = parser_current(ps)->lexeme;
    parser_advance(ps);

    std::optional<u32> rhs = parser_parse_expression(ps);
    if (!rhs.has_value()) {
        return std::nullopt;
    }
    return parser_push_expr(ps, ExprKind::Assign, op, lhs, *rhs, 0);
}

/**
 * @brief Parse a full expression (ternary + assignment + binary).
 */
std::optional<u32> parser_parse_expression(ParserState* ps)
{
    std::optional<u32> lhs = parser_parse_binary(ps, 0);
    if (!lhs.has_value()) {
        return std::nullopt;
    }

    // Ternary.
    if (parser_match(ps, TokenKind::Question)) {
        std::optional<u32> then_expr = parser_parse_expression(ps);
        if (!then_expr.has_value()) {
            return std::nullopt;
        }
        if (!parser_expect(ps, TokenKind::Colon, "':'")) {
            return std::nullopt;
        }
        std::optional<u32> else_expr = parser_parse_expression(ps);
        if (!else_expr.has_value()) {
            return std::nullopt;
        }
        lhs = parser_push_expr(ps, ExprKind::Ternary, "?", *lhs, *then_expr, *else_expr);
        return lhs;
    }

    // Assignment (right-associative via recursion on rhs).
    return parser_parse_assign(ps, *lhs);
}
