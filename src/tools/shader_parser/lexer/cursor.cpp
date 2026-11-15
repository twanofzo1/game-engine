/**
 * @file cursor.cpp
 * @brief Cursor over the source text and character-class predicates.
 */
#include "tools/shader_parser/lexer/lexer_internal.hpp"


/** @brief Return the current character, or '\0' at end of input. */
char cursor_peek(Cursor* cur)
{
    if (cur->len != 0 && cur->pos >= cur->len) {
        return '\0';
    }
    return cur->src[cur->pos];
}

/** @brief Return the character `n` bytes ahead, or '\0' past the end. */
char cursor_peek_n(Cursor* cur, u32 n)
{
    if (cur->len != 0 && cur->pos + n >= cur->len) {
        return '\0';
    }
    return cur->src[cur->pos + n];
}

/**
 * @brief Advance past the current character, updating line/column.
 * @warning Must not be called once at end of input.
 */
void cursor_advance(Cursor* cur)
{
    char c = cursor_peek(cur);
    if (c == '\0') {
        return;
    }
    cur->pos++;
    if (c == '\n') {
        cur->line++;
        cur->column = 1;
    } else {
        cur->column++;
    }
}

/** @brief True if all input has been consumed. */
bool cursor_at_end(Cursor* cur)
{
    return cursor_peek(cur) == '\0';
}


/** @brief True if `c` can appear as the first character of an identifier. */
bool lexer_is_ident_start(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/** @brief True if `c` can appear inside an identifier (after the first). */
bool lexer_is_ident_char(char c)
{
    return lexer_is_ident_start(c) || (c >= '0' && c <= '9');
}

/** @brief True if `c` is a decimal digit. */
bool lexer_is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/** @brief True if `c` is a hexadecimal digit. */
bool lexer_is_hex_digit(char c)
{
    return lexer_is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/** @brief True if `c` is ASCII whitespace. */
bool lexer_is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}
