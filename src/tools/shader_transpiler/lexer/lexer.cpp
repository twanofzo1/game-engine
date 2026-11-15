/**
 * @file lexer.cpp
 * @brief Main tokenise loop for the GLSL-like language.
 */
#include "tools/shader_transpiler/lexer/lexer.hpp"
#include "tools/shader_transpiler/lexer/lexer_internal.hpp"

#include <optional>


/** @brief Tokenise the given source into a flat token list ending in EndOfFile. */
std::expected<std::vector<Token>, LexerError> Lexer::tokenise(LexerConfig config)
{
    std::vector<Token> tokens;

    Cursor cur;
    cur.src = config.source;
    cur.len = config.source_len;

    for (;;) {
        std::optional<LexerError> err = lexer_skip_trivia(&cur);
        if (err.has_value()) {
            return std::unexpected(*err);
        }

        if (cursor_at_end(&cur)) {
            break;
        }

        u32 start_line = cur.line;
        u32 start_col = cur.column;
        char c = cursor_peek(&cur);

        // String literal.
        if (c == '"') {
            std::optional<Token> str = lexer_scan_string(&cur);
            if (!str.has_value()) {
                return std::unexpected(LexerError::UnterminatedString);
            }
            tokens.push_back(*str);
            continue;
        }

        // Identifier or keyword.
        if (lexer_is_ident_start(c)) {
            tokens.push_back(lexer_scan_identifier(&cur));
            continue;
        }

        // Numeric literal (including leading `.5` forms via Dot+digits).
        if (lexer_is_digit(c)) {
            tokens.push_back(lexer_scan_number(&cur));
            continue;
        }
        if (c == '.' && lexer_is_digit(cursor_peek_n(&cur, 1))) {
            // Float beginning with '.', e.g. `.5`.
            u32 start = cur.pos;
            cursor_advance(&cur); // '.'
            while (lexer_is_digit(cursor_peek(&cur))) {
                cursor_advance(&cur);
            }
            if (cursor_peek(&cur) == 'e' || cursor_peek(&cur) == 'E') {
                cursor_advance(&cur);
                if (cursor_peek(&cur) == '+' || cursor_peek(&cur) == '-') {
                    cursor_advance(&cur);
                }
                while (lexer_is_digit(cursor_peek(&cur))) {
                    cursor_advance(&cur);
                }
            }
            if (cursor_peek(&cur) == 'f' || cursor_peek(&cur) == 'F') {
                cursor_advance(&cur);
            }
            std::string lexeme(cur.src + start, cur.pos - start);
            tokens.push_back(Token{
                .kind = TokenKind::FloatLiteral,
                .lexeme = lexeme,
                .line = start_line,
                .column = start_col,
            });
            continue;
        }

        // Three- and two-character operators.
        std::optional<TokenKind> k = lexer_three_char_operator(c, cursor_peek_n(&cur, 1), cursor_peek_n(&cur, 2));
        if (k.has_value()) {
            cursor_advance(&cur);
            cursor_advance(&cur);
            cursor_advance(&cur);
            std::string lexeme(cur.src + cur.pos - 3, 3);
            tokens.push_back(Token{
                .kind = *k,
                .lexeme = lexeme,
                .line = start_line,
                .column = start_col,
            });
            continue;
        }
        k = lexer_two_char_operator(c, cursor_peek_n(&cur, 1));
        if (k.has_value()) {
            cursor_advance(&cur);
            cursor_advance(&cur);
            std::string lexeme(cur.src + cur.pos - 2, 2);
            tokens.push_back(Token{
                .kind = *k,
                .lexeme = lexeme,
                .line = start_line,
                .column = start_col,
            });
            continue;
        }

        // Single-character punctuators.
        k = lexer_single_char_operator(c);
        if (k.has_value()) {
            cursor_advance(&cur);
            std::string lexeme(cur.src + cur.pos - 1, 1);
            tokens.push_back(Token{
                .kind = *k,
                .lexeme = lexeme,
                .line = start_line,
                .column = start_col,
            });
            continue;
        }

        // Unknown character.
        return std::unexpected(LexerError::UnexpectedChar);
    }

    tokens.push_back(Token{
        .kind = TokenKind::EndOfFile,
        .lexeme = std::string(),
        .line = cur.line,
        .column = cur.column,
    });

    return tokens;
}
