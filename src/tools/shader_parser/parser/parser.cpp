/**
 * @file parser.cpp
 * @brief Parser entry point and token-cursor utilities.
 */
#include "tools/shader_parser/parser/parser_internal.hpp"


/** @brief Return the token at `offset` from the current position. */
const Token* parser_peek(ParserState* ps, u32 offset)
{
    u32 idx = ps->pos + offset;
    if (idx >= ps->tokens->size()) {
        return &(*ps->tokens)[ps->tokens->size() - 1]; // EndOfFile sentinel.
    }
    return &(*ps->tokens)[idx];
}

/** @brief Return the current token. */
const Token* parser_current(ParserState* ps)
{
    return parser_peek(ps, 0);
}

/** @brief Advance past the current token. */
void parser_advance(ParserState* ps)
{
    if (ps->pos < ps->tokens->size() - 1) {
        ps->pos++;
    }
}

/** @brief True if the current token kind matches `kind`. */
bool parser_check(ParserState* ps, TokenKind kind)
{
    return parser_current(ps)->kind == kind;
}

/** @brief True if the current token's lexeme matches `word`. */
bool parser_check_word(ParserState* ps, std::string word)
{
    return parser_current(ps)->lexeme == word;
}

/** @brief Consume `kind` if present; return true if it was consumed. */
bool parser_match(ParserState* ps, TokenKind kind)
{
    if (parser_check(ps, kind)) {
        parser_advance(ps);
        return true;
    }
    return false;
}

/**
 * @brief Consume the expected `kind`, recording an error if it is absent.
 * @return True if the token was present and consumed.
 */
bool parser_expect(ParserState* ps, TokenKind kind, std::string what)
{
    if (parser_check(ps, kind)) {
        parser_advance(ps);
        return true;
    }
    ps->failed_token = parser_current(ps);
    ps->failed_message = "expected " + what;
    return false;
}

/** @brief Record a failure at the current token and return it. */
ParseFailure parser_fail(ParserState* ps, ParserError error, std::string message)
{
    const Token* t = ps->failed_token ? ps->failed_token : parser_current(ps);
    return ParseFailure{
        .error = error,
        .message = message,
        .line = t->line,
        .column = t->column,
    };
}

/** @brief Parse the configured token stream into a Shader AST. */
std::expected<Shader, ParseFailure> Parser::parse(ParserConfig config)
{
    ParserState ps;
    ps.tokens = config.tokens;

    if (ps.tokens == nullptr || ps.tokens->empty()) {
        return std::unexpected(ParseFailure{
            .error = ParserError::InternalError,
            .message = "no tokens provided",
            .line = 1,
            .column = 1,
        });
    }

    // Reserve index 0 in the node vectors so that `0` unambiguously means
    // "no node" throughout the AST. Real expressions/statements start at
    // index 1.
    ps.shader.exprs.push_back(Expr{});
    ps.shader.stmts.push_back(Stmt{});

    if (auto f = parser_parse_declarations(&ps)) {
        return std::unexpected(*f);
    }

    return ps.shader;
}
