/**
 * @file shader_compiler.hpp
 * @brief Top-level API that compiles .shader source into GLSL pairs.
 *
 * Binds the lexer, parser and transpiler into a single entry point. Callers
 * pass raw `.shader` source text (a string) and receive either two GLSL
 * strings — one vertex and one fragment — or a unified error describing which
 * stage failed and where.
 */
#pragma once

#include "common/core/typedefs.hpp"

#include <expected>
#include <string>


/** @brief Which compilation stage produced the error. */
enum class CompileError : u8 {
    Success        = 0, ///< No error.
    LexError       = 1, ///< The source could not be tokenised.
    ParseError     = 2, ///< The token stream could not be parsed.
    TranspileError = 3, ///< The AST could not be transpiled.
    NoSource       = 4, ///< A null or empty source string was supplied.
};

/** @brief A compilation failure with a human-readable position. */
struct CompileFailure {
    CompileError error;   ///< Failure kind.
    std::string message;  ///< Human-readable description.
    u32 line;             ///< 1-based line of the offending token (0 if n/a).
    u32 column;           ///< 1-based column of the offending token (0 if n/a).
};

/** @brief The pair of GLSL strings produced by a successful compile. */
struct CompiledShader {
    std::string vertex_source;   ///< Complete GLSL vertex shader source.
    std::string fragment_source; ///< Complete GLSL fragment shader source.
};

/**
 * @brief Compiles `.shader` source text end-to-end.
 *
 * Runs the lexer, parser and transpiler in sequence. On success the two GLSL
 * sources are returned; on failure a CompileFailure describes the stage,
 * message and source position.
 */
struct ShaderCompiler {
    /**
     * @brief Compile `.shader` source into GLSL vertex/fragment strings.
     * @param source  NUL-terminated `.shader` source text.
     * @return The two GLSL strings, or a CompileFailure.
     */
    static std::expected<CompiledShader, CompileFailure> compile(const char* source);
};
