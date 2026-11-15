/**
 * @file scanners.cpp
 * @brief Trivia skipping and literal/identifier scanning helpers.
 */
#include "tools/shader_parser/lexer/lexer_internal.hpp"


/**
 * @brief Skip whitespace and comments, returning an error on unterminated
 *        block comments.
 */
std::optional<LexerError> lexer_skip_trivia(Cursor* cur)
{
    for (;;) {
        // Skip runs of whitespace.
        while (!cursor_at_end(cur) && lexer_is_space(cursor_peek(cur))) {
            cursor_advance(cur);
        }

        char c = cursor_peek(cur);
        if (c == '/' && cursor_peek_n(cur, 1) == '/') {
            // Line comment: skip to end of line or input.
            cursor_advance(cur);
            cursor_advance(cur);
            while (!cursor_at_end(cur) && cursor_peek(cur) != '\n') {
                cursor_advance(cur);
            }
            continue;
        }
        if (c == '/' && cursor_peek_n(cur, 1) == '*') {
            cursor_advance(cur);
            cursor_advance(cur);
            // Block comment: skip until the closing `*/`.
            bool closed = false;
            while (!cursor_at_end(cur)) {
                if (cursor_peek(cur) == '*' && cursor_peek_n(cur, 1) == '/') {
                    cursor_advance(cur);
                    cursor_advance(cur);
                    closed = true;
                    break;
                }
                cursor_advance(cur);
            }
            if (!closed) {
                return LexerError::UnterminatedComment;
            }
            continue;
        }
        return std::nullopt; // Not trivia: caller handles the token.
    }
}

/**
 * @brief Lex a number literal (integer or float) starting at `cur`.
 *
 * Handles decimal, hexadecimal and octal integer literals with optional
 * `u`/`U` and `lf`/`LF` suffixes, and float literals with exponents.
 */
Token lexer_scan_number(Cursor* cur)
{
    u32 start = cur->pos;
    u32 start_line = cur->line;
    u32 start_col = cur->column;

    bool is_float = false;
    bool is_double = false;
    bool is_uint = false;

    if (cursor_peek(cur) == '0' && (cursor_peek_n(cur, 1) == 'x' || cursor_peek_n(cur, 1) == 'X')) {
        // Hexadecimal integer.
        cursor_advance(cur); // '0'
        cursor_advance(cur); // 'x'
        while (lexer_is_hex_digit(cursor_peek(cur))) {
            cursor_advance(cur);
        }
    } else {
        // Decimal (or leading-0 octal) integer or float.
        while (lexer_is_digit(cursor_peek(cur))) {
            cursor_advance(cur);
        }
        if (cursor_peek(cur) == '.' && lexer_is_digit(cursor_peek_n(cur, 1))) {
            is_float = true;
            cursor_advance(cur); // '.'
            while (lexer_is_digit(cursor_peek(cur))) {
                cursor_advance(cur);
            }
        }
        if (cursor_peek(cur) == 'e' || cursor_peek(cur) == 'E') {
            is_float = true;
            cursor_advance(cur);
            if (cursor_peek(cur) == '+' || cursor_peek(cur) == '-') {
                cursor_advance(cur);
            }
            while (lexer_is_digit(cursor_peek(cur))) {
                cursor_advance(cur);
            }
        }
        // Float suffix.
        if (cursor_peek(cur) == 'f' || cursor_peek(cur) == 'F') {
            is_float = true;
            cursor_advance(cur);
        }
    }

    if (!is_float) {
        // Unsigned suffix `u`/`U` (only valid on an integer literal).
        if (cursor_peek(cur) == 'u' || cursor_peek(cur) == 'U') {
            is_uint = true;
            cursor_advance(cur);
        }
    }

    // Double suffix `lf`/`LF`, valid on any numeric literal.
    if (cursor_peek(cur) == 'l' || cursor_peek(cur) == 'L') {
        cursor_advance(cur);
        if (cursor_peek(cur) == 'f' || cursor_peek(cur) == 'F') {
            cursor_advance(cur);
        }
        is_double = true;
        is_float = true;
        is_uint = false;
    }

    std::string lexeme(cur->src + start, cur->pos - start);

    TokenKind kind;
    if (is_double) {
        kind = TokenKind::DoubleLiteral;
    } else if (is_uint) {
        kind = TokenKind::UintLiteral;
    } else if (is_float) {
        kind = TokenKind::FloatLiteral;
    } else {
        kind = TokenKind::IntLiteral;
    }

    return Token{
        .kind = kind,
        .lexeme = lexeme,
        .line = start_line,
        .column = start_col,
    };
}

/**
 * @brief Lex a double-quoted string literal starting at `cur`.
 *
 * The lexeme includes the surrounding quotes and preserves escape sequences
 * verbatim (they are not resolved here). An unterminated string returns
 * LexerError::UnterminatedString.
 */
std::optional<Token> lexer_scan_string(Cursor* cur)
{
    u32 start = cur->pos;
    u32 start_line = cur->line;
    u32 start_col = cur->column;

    cursor_advance(cur); // Opening quote.

    bool closed = false;
    while (!cursor_at_end(cur)) {
        char c = cursor_peek(cur);
        if (c == '"') {
            cursor_advance(cur);
            closed = true;
            break;
        }
        if (c == '\\') {
            // Skip the escaped character so `\"` is not seen as a close.
            cursor_advance(cur);
            if (!cursor_at_end(cur)) {
                cursor_advance(cur);
            }
            continue;
        }
        cursor_advance(cur);
    }

    if (!closed) {
        return std::nullopt;
    }

    std::string lexeme(cur->src + start, cur->pos - start);
    return Token{
        .kind = TokenKind::StringLiteral,
        .lexeme = lexeme,
        .line = start_line,
        .column = start_col,
    };
}

/**
 * @brief Lex an identifier or keyword starting at `cur`.
 */
Token lexer_scan_identifier(Cursor* cur)
{
    u32 start = cur->pos;
    u32 start_line = cur->line;
    u32 start_col = cur->column;

    cursor_advance(cur); // First character is known to be an identifier start.
    while (lexer_is_ident_char(cursor_peek(cur))) {
        cursor_advance(cur);
    }

    std::string lexeme(cur->src + start, cur->pos - start);

    TokenKind kind = TokenKind::Identifier;
    if (lexer_is_keyword(lexeme)) {
        kind = TokenKind::Keyword;
    } else if (lexer_is_type_keyword(lexeme)) {
        kind = TokenKind::TypeKeyword;
    }

    return Token{
        .kind = kind,
        .lexeme = lexeme,
        .line = start_line,
        .column = start_col,
    };
}
