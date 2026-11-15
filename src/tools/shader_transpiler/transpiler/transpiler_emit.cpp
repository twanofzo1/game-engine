/**
 * @file transpiler_emit.cpp
 * @brief Emitter primitives and DSL-to-GLSL type name mapping.
 */
#include "tools/shader_transpiler/transpiler/transpiler_internal.hpp"


/** @brief Append a single indented line to the emitter. */
void emit_line(Emitter* em, std::string line)
{
    for (u32 i = 0; i < em->indent; i++) {
        em->out += "    ";
    }
    em->out += line;
    em->out += "\n";
}

/** @brief Append a raw (no indentation) line to the emitter. */
void emit_raw(Emitter* em, std::string line)
{
    em->out += line;
    em->out += "\n";
}

/** @brief Increase the emitter indentation level. */
void emit_indent(Emitter* em)
{
    em->indent++;
}

/** @brief Decrease the emitter indentation level. */
void emit_dedent(Emitter* em)
{
    if (em->indent > 0) {
        em->indent--;
    }
}

/**
 * @brief Map a DSL type name to its GLSL equivalent.
 *
 * The DSL uses PascalCase engine types; these collapse to lowercase GLSL
 * built-ins. Lowercase GLSL types already present in the source pass through
 * unchanged.
 */
std::string transpile_type(std::string name)
{
    if (name == "Vec2")     return "vec2";
    if (name == "Vec3")     return "vec3";
    if (name == "Vec4")     return "vec4";
    if (name == "Color")    return "vec4";
    if (name == "Mat2")     return "mat2";
    if (name == "Mat3")     return "mat3";
    if (name == "Mat4")     return "mat4";
    if (name == "Texture2D" || name == "Tex") return "sampler2D";
    return name;
}
