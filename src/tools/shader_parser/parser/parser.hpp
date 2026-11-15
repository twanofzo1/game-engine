/**
 * @file parser.hpp
 * @brief Recursive-descent parser for the shader language.
 *
 * Consumes a token stream produced by the lexer and builds a Shader AST.
 * Errors are reported as a ParseFailure carrying the offending token's
 * line/column so callers can surface useful diagnostics.
 */
#pragma once

#include "tools/shader_parser/parser/ast.hpp"
#include "tools/shader_parser/lexer/token.hpp"

#include <expected>
#include <string>
#include <vector>


/** @brief Configuration for a parse run. */
struct ParserConfig {
    const std::vector<Token>* tokens = nullptr; ///< Token list from the lexer.
};

/** @brief Error codes produced by the parser. */
enum class ParserError : u8 {
    Success           = 0, ///< No error.
    UnexpectedToken   = 1, ///< A token did not fit the current grammar rule.
    UnterminatedBlock = 2, ///< A `{` block was never closed.
    ExpectedIdentifier = 3, ///< A name was required but not found.
    ExpectedType      = 4, ///< A type name was required but not found.
    InternalError     = 5, ///< A programming error inside the parser.
};

/** @brief A parser failure, including a human-readable position. */
struct ParseFailure {
    ParserError error;    ///< Failure kind.
    std::string message;  ///< Human-readable description.
    u32 line;             ///< 1-based line of the offending token.
    u32 column;           ///< 1-based column of the offending token.
};

/**
 * @brief Recursive-descent parser over a pre-lexed token stream.
 */
struct Parser {
    /** @brief Parse the configured token stream into a Shader AST. */
    static std::expected<Shader, ParseFailure> parse(ParserConfig config);
};
