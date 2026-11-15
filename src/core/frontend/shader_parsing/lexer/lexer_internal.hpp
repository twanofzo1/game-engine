/**
 * @file lexer_internal.hpp
 * @brief Internal declarations shared by the lexer translation units.
 *
 * These helpers are free functions in the global namespace, split across
 * several small .cpp files so each stays under the project's line budget.
 * The declarations live here so every translation unit sees the same
 * signatures without pulling in the public Lexer interface.
 */
#pragma once

#include "core/frontend/shader_parsing/lexer/token.hpp"

#include <optional>


/** @brief Cursor over the source text tracking line/column position. */
struct Cursor {
    const char* src;        ///< Start of source buffer.
    u32 len;                ///< Source length (or 0 for NUL-terminated).
    u32 pos = 0;            ///< Byte offset of the current character.
    u32 line = 1;           ///< 1-based line of the current character.
    u32 column = 1;         ///< 1-based column of the current character.
};


/** @brief Return the current character, or '\0' at end of input. */
char cursor_peek(Cursor* cur);

/** @brief Return the character `n` bytes ahead, or '\0' past the end. */
char cursor_peek_n(Cursor* cur, u32 n);

/**
 * @brief Advance past the current character, updating line/column.
 * @warning Must not be called once at end of input.
 */
void cursor_advance(Cursor* cur);

/** @brief True if all input has been consumed. */
bool cursor_at_end(Cursor* cur);


/** @brief True if `c` can appear as the first character of an identifier. */
bool lexer_is_ident_start(char c);

/** @brief True if `c` can appear inside an identifier (after the first). */
bool lexer_is_ident_char(char c);

/** @brief True if `c` is a decimal digit. */
bool lexer_is_digit(char c);

/** @brief True if `c` is a hexadecimal digit. */
bool lexer_is_hex_digit(char c);

/** @brief True if `c` is ASCII whitespace. */
bool lexer_is_space(char c);


/** @brief True if `word` is one of the language's keywords (non-type). */
bool lexer_is_keyword(std::string word);

/** @brief True if `word` is one of the language's built-in type names. */
bool lexer_is_type_keyword(std::string word);


/** @brief Return the two-char operator kind, or nullopt if none matches. */
std::optional<TokenKind> lexer_two_char_operator(char a, char b);

/** @brief Return the three-char operator kind, or nullopt if none matches. */
std::optional<TokenKind> lexer_three_char_operator(char a, char b, char c);

/** @brief Return the single-char punctuator/operator kind, or nullopt. */
std::optional<TokenKind> lexer_single_char_operator(char c);


/**
 * @brief Skip whitespace and comments, returning an error on unterminated
 *        block comments.
 */
std::optional<LexerError> lexer_skip_trivia(Cursor* cur);

/**
 * @brief Lex a number literal (integer or float) starting at `cur`.
 */
Token lexer_scan_number(Cursor* cur);

/**
 * @brief Lex an identifier or keyword starting at `cur`.
 */
Token lexer_scan_identifier(Cursor* cur);

/**
 * @brief Lex a double-quoted string literal starting at `cur`.
 *
 * The lexeme includes the surrounding quotes and preserves escape sequences
 * verbatim (they are not resolved here). An unterminated string returns
 * LexerError::UnterminatedString.
 */
std::optional<Token> lexer_scan_string(Cursor* cur);
