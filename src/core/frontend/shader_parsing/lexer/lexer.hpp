/**
 * @file lexer.hpp
 * @brief Tokenises GLSL-like source text into a token stream.
 */
#pragma once

#include "core/frontend/shader_parsing/lexer/token.hpp"

#include <expected>
#include <vector>


/**
 * @brief Configuration for a lexer run.
 */
struct LexerConfig {
    const char* source = nullptr; ///< NUL-terminated source to tokenise (required).
    u32 source_len = 0;           ///< Length of source, or 0 to scan until NUL.
};

/** @brief State shared across a single tokenise call. */
struct Lexer {
    static std::expected<std::vector<Token>, LexerError> tokenise(LexerConfig config);
};
