/**
 * @file parser_decl.cpp
 * @brief Parsing of function declarations and the declaration dispatcher.
 */
#include "core/frontend/shader_parsing/parser/parser_internal.hpp"


/**
 * @brief Parse a function parameter `Type name`.
 */
static std::optional<ParseFailure> parser_parse_param(ParserState* ps)
{
    if (!parser_is_type_token(parser_current(ps)->kind)) {
        return parser_fail(ps, ParserError::ExpectedType, "expected a parameter type");
    }
    std::string type = parser_current(ps)->lexeme;
    parser_advance(ps);

    if (!parser_check(ps, TokenKind::Identifier)) {
        return parser_fail(ps, ParserError::ExpectedIdentifier, "expected a parameter name");
    }
    std::string name = parser_current(ps)->lexeme;
    parser_advance(ps);

    Param p;
    p.type = type;
    p.name = name;
    ps->shader.params.push_back(p);
    return std::nullopt;
}

/** @brief The name, return type and stage of a function being parsed. */
struct FunctionSignature {
    std::string name;   ///< Function name.
    std::string type;   ///< Return type name.
    Stage stage;        ///< Entry-point stage (or None).
};

/**
 * @brief Detect a function's name, return type and stage from its first
 * tokens, advancing past the detected signature prefix.
 */
static std::optional<FunctionSignature> parser_detect_signature(ParserState* ps)
{
    FunctionSignature sig;
    sig.name = "";
    sig.type = "void";
    sig.stage = Stage::None;

    const Token* first = parser_current(ps);

    // Detect an entry point: `Vertex` or `Fragment` followed by `(`.
    if (first->kind == TokenKind::Identifier
        && (first->lexeme == "Vertex" || first->lexeme == "Fragment")
        && parser_peek(ps, 1)->kind == TokenKind::LeftParen) {
        sig.stage = (first->lexeme == "Vertex") ? Stage::Vertex : Stage::Fragment;
        sig.name = first->lexeme;
        parser_advance(ps);
        return sig;
    }
    // Typed helper: `ReturnType name(...)`.
    if (parser_is_type_token(first->kind)
        && parser_peek(ps, 1)->kind == TokenKind::Identifier
        && parser_peek(ps, 2)->kind == TokenKind::LeftParen) {
        sig.type = first->lexeme;
        parser_advance(ps);
        sig.name = parser_current(ps)->lexeme;
        parser_advance(ps);
        return sig;
    }
    // Plain helper with implicit void return: `name(...)`.
    if (first->kind == TokenKind::Identifier
        && parser_peek(ps, 1)->kind == TokenKind::LeftParen) {
        sig.name = first->lexeme;
        parser_advance(ps);
        return sig;
    }
    return std::nullopt;
}

/**
 * @brief Parse a function definition and register it in the AST.
 */
static std::optional<ParseFailure> parser_parse_function(ParserState* ps)
{
    std::optional<FunctionSignature> sig = parser_detect_signature(ps);
    if (!sig.has_value()) {
        return parser_fail(ps, ParserError::UnexpectedToken, "expected a function definition");
    }

    // Parameter list.
    u32 param_start = static_cast<u32>(ps->shader.params.size());
    if (!parser_expect(ps, TokenKind::LeftParen, "'('")) {
        return parser_fail(ps, ParserError::UnexpectedToken, ps->failed_message);
    }
    while (!parser_check(ps, TokenKind::RightParen)) {
        if (auto f = parser_parse_param(ps)) {
            return f;
        }
        if (!parser_match(ps, TokenKind::Comma)) {
            break;
        }
    }
    if (!parser_expect(ps, TokenKind::RightParen, "')'")) {
        return parser_fail(ps, ParserError::UnexpectedToken, ps->failed_message);
    }
    u32 param_count = static_cast<u32>(ps->shader.params.size()) - param_start;

    // Body block.
    std::optional<u32> body = parser_parse_statement(ps);
    if (!body.has_value()) {
        return parser_fail(ps, ParserError::UnexpectedToken, "expected a function body");
    }

    ps->shader.fn_names.push_back(sig->name);
    ps->shader.fn_types.push_back(sig->type);
    ps->shader.fn_stages.push_back(sig->stage);
    ps->shader.fn_param_start.push_back(param_start);
    ps->shader.fn_param_count.push_back(param_count);
    ps->shader.fn_body.push_back(*body);
    return std::nullopt;
}

/**
 * @brief Parse a single top-level declaration.
 */
static std::optional<ParseFailure> parser_parse_declaration(ParserState* ps)
{
    if (parser_check(ps, TokenKind::Hash)) {
        return parser_parse_include(ps);
    }
    if (parser_check_word(ps, "uniform")) {
        return parser_parse_uniform(ps);
    }
    return parser_parse_function(ps);
}

/**
 * @brief Parse all top-level declarations until EndOfFile.
 */
std::optional<ParseFailure> parser_parse_declarations(ParserState* ps)
{
    while (!parser_check(ps, TokenKind::EndOfFile)) {
        if (auto f = parser_parse_declaration(ps)) {
            return f;
        }
    }
    return std::nullopt;
}
