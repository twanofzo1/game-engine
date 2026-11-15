/**
 * @file transpiler.hpp
 * @brief Transpiles a parsed shader AST into GLSL vertex/fragment sources.
 *
 * The source language is a C-like shader DSL with `Vertex`/`Fragment` entry
 * points, `uniform` globals, and engine-specific types (`Vec2`, `Vec4`,
 * `Color`, ...). The transpiler emits complete GLSL programs, injecting
 * default boilerplate (vertex attributes, a view-projection uniform, a
 * texture sampler, varying plumbing) that user code overrides where it
 * defines its own `Vertex`/`Fragment` bodies and uniforms.
 */
#pragma once

#include "tools/shader_transpiler/parser/ast.hpp"

#include <expected>
#include <string>


/** @brief Error codes produced by the transpiler. */
enum class TranspileError : u8 {
    Success          = 0, ///< No error.
    MissingVertex    = 1, ///< No `Vertex(...)` entry point was defined.
    MissingFragment  = 2, ///< No `Fragment(...)` entry point was defined.
    InternalError    = 3, ///< An invariant was violated internally.
};

/** @brief The two GLSL strings produced by a successful transpile. */
struct TranspileResult {
    std::string vertex_source;   ///< Complete GLSL vertex shader source.
    std::string fragment_source; ///< Complete GLSL fragment shader source.
};

/**
 * @brief Transpiles a parsed shader AST into GLSL.
 */
struct Transpiler {
    /**
     * @brief Emit GLSL vertex and fragment sources from a parsed shader.
     * @param shader  The parsed AST produced by Parser::parse.
     * @return The two GLSL sources, or a TranspileError.
     */
    static std::expected<TranspileResult, TranspileError> transpile(const Shader* shader);
};
