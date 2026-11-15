/**
 * @file transpiler_internal.hpp
 * @brief Internal declarations shared by the transpiler translation units.
 */
#pragma once

#include "tools/shader_transpiler/transpiler/transpiler.hpp"

#include <optional>


/**
 * @brief Mutable emitter state shared across a transpile run.
 *
 * The emitter builds GLSL source by appending lines to an output buffer,
 * tracking indentation and which shader stage is currently being written.
 */
struct Emitter {
    std::string out;   ///< Accumulated GLSL source.
    u32 indent = 0;    ///< Current indentation level (in 4-space units).
};

/** @brief Which kind of shader stage is being emitted. */
enum class StageKind : u8 {
    Vertex,   ///< Vertex shader (vertex_output / gl_Position).
    Fragment, ///< Fragment shader (fragment_output / FragColor).
};

/** @brief Append a single indented line to the emitter. */
void emit_line(Emitter* em, std::string line);

/** @brief Append a raw (no indentation) line to the emitter. */
void emit_raw(Emitter* em, std::string line);

/** @brief Increase the emitter indentation level. */
void emit_indent(Emitter* em);

/** @brief Decrease the emitter indentation level. */
void emit_dedent(Emitter* em);

/**
 * @brief Map a DSL type name to its GLSL equivalent.
 * @return The mapped name, or the input unchanged if no mapping exists.
 */
std::string transpile_type(std::string name);

/**
 * @brief Emit an expression to GLSL text.
 * @param shader  The AST owning the expression storage.
 * @param expr    Index of the expression to emit.
 */
std::string transpile_expr(const Shader* shader, u32 expr);

/**
 * @brief Emit a statement (and any nested statements) to GLSL.
 * @param shader  The AST owning the statement storage.
 * @param stmt    Index of the statement (usually a Block) to emit.
 * @param stage   Whether this is a vertex or fragment stage.
 */
void transpile_statement(const Shader* shader, Emitter* em, u32 stmt, StageKind stage);

/**
 * @brief Emit the uniforms into the current shader's global scope.
 *
 * Uniforms are declared in both stages so either entry point may use them.
 */
void transpile_uniforms(const Shader* shader, Emitter* em);

/**
 * @brief Emit the complete vertex shader.
 */
std::string transpile_vertex(const Shader* shader);

/**
 * @brief Emit the complete fragment shader.
 */
std::string transpile_fragment(const Shader* shader);
