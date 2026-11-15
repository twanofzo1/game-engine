/**
 * @file operators.cpp
 * @brief Operator and punctuator token classification.
 */
#include "core/frontend/shader_parsing/lexer/lexer_internal.hpp"


/** @brief Return the two-char operator kind, or nullopt if none matches. */
std::optional<TokenKind> lexer_two_char_operator(char a, char b)
{
    if (a == '+' && b == '+') return TokenKind::PlusPlus;
    if (a == '-' && b == '-') return TokenKind::MinusMinus;
    if (a == '+' && b == '=') return TokenKind::PlusAssign;
    if (a == '-' && b == '=') return TokenKind::MinusAssign;
    if (a == '*' && b == '=') return TokenKind::StarAssign;
    if (a == '/' && b == '=') return TokenKind::SlashAssign;
    if (a == '%' && b == '=') return TokenKind::PercentAssign;
    if (a == '=' && b == '=') return TokenKind::Equal;
    if (a == '!' && b == '=') return TokenKind::NotEqual;
    if (a == '<' && b == '=') return TokenKind::LessEqual;
    if (a == '>' && b == '=') return TokenKind::GreaterEqual;
    if (a == '&' && b == '&') return TokenKind::LogicalAnd;
    if (a == '|' && b == '|') return TokenKind::LogicalOr;
    if (a == '^' && b == '^') return TokenKind::LogicalXor;
    if (a == '<' && b == '<') return TokenKind::ShiftLeft;
    if (a == '>' && b == '>') return TokenKind::ShiftRight;
    if (a == '&' && b == '=') return TokenKind::AndAssign;
    if (a == '|' && b == '=') return TokenKind::OrAssign;
    if (a == '^' && b == '=') return TokenKind::XorAssign;
    return std::nullopt;
}

/** @brief Return the three-char operator kind, or nullopt if none matches. */
std::optional<TokenKind> lexer_three_char_operator(char a, char b, char c)
{
    if (a == '<' && b == '<' && c == '=') return TokenKind::ShiftLeftAssign;
    if (a == '>' && b == '>' && c == '=') return TokenKind::ShiftRightAssign;
    return std::nullopt;
}

/** @brief Return the single-char punctuator/operator kind, or nullopt. */
std::optional<TokenKind> lexer_single_char_operator(char c)
{
    switch (c) {
    case '(': return TokenKind::LeftParen;
    case ')': return TokenKind::RightParen;
    case '{': return TokenKind::LeftBrace;
    case '}': return TokenKind::RightBrace;
    case '[': return TokenKind::LeftBracket;
    case ']': return TokenKind::RightBracket;
    case ',': return TokenKind::Comma;
    case ';': return TokenKind::Semicolon;
    case '.': return TokenKind::Dot;
    case ':': return TokenKind::Colon;
    case '#': return TokenKind::Hash;
    case '=': return TokenKind::Assign;
    case '+': return TokenKind::Plus;
    case '-': return TokenKind::Minus;
    case '*': return TokenKind::Star;
    case '/': return TokenKind::Slash;
    case '%': return TokenKind::Percent;
    case '^': return TokenKind::Caret;
    case '!': return TokenKind::Bang;
    case '~': return TokenKind::Tilde;
    case '?': return TokenKind::Question;
    case '&': return TokenKind::Ampersand;
    case '|': return TokenKind::Pipe;
    case '<': return TokenKind::Less;
    case '>': return TokenKind::Greater;
    default:  return std::nullopt;
    }
}
