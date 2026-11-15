/**
 * @file token.hpp
 * @brief Token model produced by the GLSL-like lexer.
 */
#pragma once

#include "core/typedefs.hpp"

#include <string>


/** @brief The category a single lexed token falls into. */
enum class TokenKind : u8 {
    EndOfFile,     ///< Marks the end of the input (no lexeme).
    Identifier,    ///< A user-defined or keyword name.
    Keyword,       ///< A reserved word (if, else, for, return, ...).
    TypeKeyword,   ///< A reserved type name (float, vec3, sampler2D, ...).
    IntLiteral,    ///< A decimal integer literal.
    UintLiteral,   ///< An unsigned integer literal (suffixed with `u`).
    FloatLiteral,  ///< A floating-point literal.
    DoubleLiteral, ///< A double-precision literal (suffixed with `lf`/`LF`).
    StringLiteral, ///< A double-quoted string literal, escapes not resolved.

    // Punctuation and operators.
    Hash,          ///< `#` (preprocessor directive start).
    LeftParen,     ///< `(`
    RightParen,    ///< `)`
    LeftBrace,     ///< `{`
    RightBrace,    ///< `}`
    LeftBracket,   ///< `[`
    RightBracket,  ///< `]`
    Comma,         ///< `,`
    Semicolon,     ///< `;`
    Dot,           ///< `.`
    Colon,         ///< `:`

    Assign,        ///< `=`
    Plus,          ///< `+`
    Minus,         ///< `-`
    Star,          ///< `*`
    Slash,         ///< `/`
    Percent,       ///< `%`
    Caret,         ///< `^`
    Bang,          ///< `!`
    Tilde,         ///< `~`
    Question,      ///< `?`
    Ampersand,     ///< `&`
    Pipe,          ///< `|`

    PlusPlus,      ///< `++`
    MinusMinus,    ///< `--`

    PlusAssign,       ///< `+=`
    MinusAssign,      ///< `-=`
    StarAssign,       ///< `*=`
    SlashAssign,      ///< `/=`
    PercentAssign,    ///< `%=`

    Equal,             ///< `==`
    NotEqual,          ///< `!=`
    Less,              ///< `<`
    LessEqual,         ///< `<=`
    Greater,           ///< `>`
    GreaterEqual,      ///< `>=`
    LogicalAnd,        ///< `&&`
    LogicalOr,         ///< `||`
    LogicalXor,        ///< `^^`
    ShiftLeft,         ///< `<<`
    ShiftRight,        ///< `>>`
    ShiftLeftAssign,   ///< `<<=`
    ShiftRightAssign,  ///< `>>=`
    AndAssign,         ///< `&=`
    OrAssign,          ///< `|=`
    XorAssign,         ///< `^=`
};

/**
 * @brief A single lexed token: its category, source text and position.
 *
 * The lexeme points into the source buffer for the lifetime of a tokenise
 * call; it is a view rather than an owned copy. Keywords are emitted with
 * their canonical spelling preserved.
 */
struct Token {
    TokenKind kind;                      ///< Category of this token.
    std::string lexeme;                  ///< Source text of this token (copied).
    u32 line;                            ///< 1-based source line of the token start.
    u32 column;                          ///< 1-based source column of the token start.
};

/** @brief Error codes produced by the lexer. */
enum class LexerError : u8 {
    Success           = 0, ///< No error.
    UnexpectedChar    = 1, ///< A character has no valid token starting form.
    UnterminatedComment = 2, ///< A `/*` comment was never closed.
    UnterminatedString  = 3, ///< A `"` string literal was never closed.
};
