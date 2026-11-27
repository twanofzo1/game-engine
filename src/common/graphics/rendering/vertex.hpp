/**
 * @file vertex.hpp
 * @brief Vertex layout shared by the renderer and its shaders.
 */
#pragma once

#include "common/core/typedefs.hpp"

/// @brief A single vertex with position, texture coordinates and color.
struct Vertex {
    f32 x; ///< Position X in world space.
    f32 y; ///< Position Y in world space.
    f32 z; ///< Position Z in world space.
    f32 u; ///< Texture coordinate U.
    f32 v; ///< Texture coordinate V.
    f32 r; ///< Color red component.
    f32 g; ///< Color green component.
    f32 b; ///< Color blue component.
    f32 a; ///< Color alpha component.
};

constexpr u32 VERTICES_PER_QUAD = 4;
constexpr u32 INDICES_PER_QUAD = 6;
