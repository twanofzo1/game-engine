/**
 * @file parser_internal.hpp
 * @brief Internal declarations shared by the parser translation units.
 */
#pragma once

#include "tools/shader_parser/parser/parser.hpp"
#include "tools/shader_parser/parser/ast.hpp"
#include "tools/shader_parser/lexer/token.hpp"

#include <optional>


/**
 * @brief Recursive-descent parser state: a cursor over the token stream plus
 * the AST being built.
 */
struct ParserState {
    const std::vector<Token>* tokens; ///< Token stream being consumed.
    u32 pos = 0;                      ///< Index of the current token.
    Shader shader;                    ///< AST under construction.
    const Token* failed_token = nullptr; ///< Token that triggered the last error.
    std::string failed_message;       ///< Message for the last error.
};


/** @brief Return the token at `offset` from the current position. */
const Token* parser_peek(ParserState* ps, u32 offset);

/** @brief Return the current token. */
const Token* parser_current(ParserState* ps);

/** @brief Advance past the current token. */
void parser_advance(ParserState* ps);

/** @brief True if the current token kind matches `kind`. */
bool parser_check(ParserState* ps, TokenKind kind);

/** @brief True if the current token's lexeme matches `word`. */
bool parser_check_word(ParserState* ps, std::string word);

/** @brief Consume `kind` if present; return true if it was consumed. */
bool parser_match(ParserState* ps, TokenKind kind);

/**
 * @brief Consume the expected `kind`, recording an error if it is absent.
 * @return True if the token was present and consumed.
 */
bool parser_expect(ParserState* ps, TokenKind kind, std::string what);

/** @brief Record a failure at the current token and return it. */
ParseFailure parser_fail(ParserState* ps, ParserError error, std::string message);

/** @brief Push an expression node and return its index. */
u32 parser_push_expr(ParserState* ps, ExprKind kind, std::string text,
                     u32 a, u32 b, u32 c);

/** @brief Push a statement node and return its index. */
u32 parser_push_stmt(ParserState* ps, StmtKind kind, u32 a, u32 b, u32 c, u32 d);

/** @brief Parse the top-level declarations into the AST. */
std::optional<ParseFailure> parser_parse_declarations(ParserState* ps);

/** @brief Parse a full expression (assignment level). */
std::optional<u32> parser_parse_expression(ParserState* ps);

/** @brief Parse a primary expression. */
std::optional<u32> parser_parse_primary(ParserState* ps);

/** @brief Parse a postfix expression. */
std::optional<u32> parser_parse_postfix(ParserState* ps);

/** @brief Parse a statement. */
std::optional<u32> parser_parse_statement(ParserState* ps);

// Statement sub-parsers (defined in parser_stmt_control.cpp).
std::optional<u32> parser_parse_block(ParserState* ps);
std::optional<u32> parser_parse_return(ParserState* ps);
std::optional<u32> parser_parse_if(ParserState* ps);
std::optional<u32> parser_parse_for(ParserState* ps);
std::optional<u32> parser_parse_while(ParserState* ps);

// Type-name predicate shared by the declaration parsers.
bool parser_is_type_token(TokenKind kind);

// Top-level declaration parsers (defined in parser_top.cpp).
std::optional<ParseFailure> parser_parse_include(ParserState* ps);
std::optional<ParseFailure> parser_parse_uniform(ParserState* ps);
