/**
 * @file parser_top.cpp
 * @brief Parsing of includes, uniforms and the type-name predicate.
 */
#include "tools/shader_transpiler/parser/parser_internal.hpp"


/** @brief True if a token kind starts a type name position. */
bool parser_is_type_token(TokenKind kind)
{
    return kind == TokenKind::TypeKeyword || kind == TokenKind::Identifier;
}

/**
 * @brief Parse a `#include "path"` directive.
 */
std::optional<ParseFailure> parser_parse_include(ParserState* ps)
{
    parser_advance(ps); // '#'

    // Accept `#include` with no space, or `#` followed by an identifier.
    if (!parser_check_word(ps, "include")) {
        return parser_fail(ps, ParserError::UnexpectedToken, "expected 'include' after '#'");
    }
    parser_advance(ps); // 'include'

    if (!parser_check(ps, TokenKind::StringLiteral)) {
        return parser_fail(ps, ParserError::UnexpectedToken, "expected a string literal in #include");
    }

    const Token* str = parser_current(ps);
    parser_advance(ps);

    Decl d;
    d.kind = DeclKind::Include;
    d.name = str->lexeme; // Includes surrounding quotes.
    d.type = "";
    d.stage = Stage::None;
    d.a = 0;
    d.b = 0;
    ps->shader.decls.push_back(d);
    return std::nullopt;
}

/**
 * @brief Parse a `uniform Type name;` declaration.
 */
std::optional<ParseFailure> parser_parse_uniform(ParserState* ps)
{
    parser_advance(ps); // 'uniform'

    if (!parser_is_type_token(parser_current(ps)->kind)) {
        return parser_fail(ps, ParserError::ExpectedType, "expected a type after 'uniform'");
    }
    std::string type = parser_current(ps)->lexeme;
    parser_advance(ps);

    if (!parser_check(ps, TokenKind::Identifier)) {
        return parser_fail(ps, ParserError::ExpectedIdentifier, "expected a uniform name");
    }
    std::string name = parser_current(ps)->lexeme;
    parser_advance(ps);

    if (!parser_expect(ps, TokenKind::Semicolon, "';'")) {
        return parser_fail(ps, ParserError::UnexpectedToken, ps->failed_message);
    }

    Decl d;
    d.kind = DeclKind::Uniform;
    d.name = name;
    d.type = type;
    d.stage = Stage::None;
    d.a = 0;
    d.b = 0;
    ps->shader.decls.push_back(d);
    return std::nullopt;
}
