/**
 * @file keywords.cpp
 * @brief Reserved-word classification for the GLSL-like language.
 */
#include "tools/shader_parser/lexer/lexer_internal.hpp"


/** @brief True if `word` is one of the language's keywords (non-type). */
bool lexer_is_keyword(std::string word)
{
    static const char* keywords[] = {
        "if", "else", "for", "while", "do", "switch", "case", "default",
        "break", "continue", "return", "discard", "struct", "uniform",
        "in", "out", "inout", "const", "layout", "attribute", "varying",
        "flat", "smooth", "noperspective", "centroid", "sample", "precision",
        "highp", "mediump", "lowp", "invariant", "precise", "shared",
        "packed", "buffer", "coherent", "volatile", "restrict", "readonly",
        "writeonly", "true", "false",
    };
    for (u32 i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (word == keywords[i]) {
            return true;
        }
    }
    return false;
}

/** @brief True if `word` is one of the language's built-in type names. */
bool lexer_is_type_keyword(std::string word)
{
    static const char* types[] = {
        "void", "bool", "int", "uint", "float", "double",
        "bvec2", "bvec3", "bvec4",
        "ivec2", "ivec3", "ivec4",
        "uvec2", "uvec3", "uvec4",
        "vec2", "vec3", "vec4",
        "dvec2", "dvec3", "dvec4",
        "mat2", "mat3", "mat4",
        "mat2x2", "mat2x3", "mat2x4",
        "mat3x2", "mat3x3", "mat3x4",
        "mat4x2", "mat4x3", "mat4x4",
        "dmat2", "dmat3", "dmat4",
        "sampler1D", "sampler2D", "sampler3D", "samplerCube",
        "sampler1DShadow", "sampler2DShadow", "samplerCubeShadow",
        "sampler1DArray", "sampler2DArray",
        "sampler2DArrayShadow", "samplerCubeArray", "samplerCubeArrayShadow",
        "isampler1D", "isampler2D", "isampler3D", "isamplerCube",
        "usampler1D", "usampler2D", "usampler3D", "usamplerCube",
        "sampler2DRect", "sampler2DRectShadow",
    };
    for (u32 i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
        if (word == types[i]) {
            return true;
        }
    }
    return false;
}
