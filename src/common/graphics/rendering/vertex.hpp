/**
 * @file vertex.hpp
 * @brief Vertex layout shared by the renderer and its shaders.
 */
#pragma once

#include "common/core/typedefs.hpp"

/** One vertex: position, texture coordinate and RGBA color. */
struct Vertex {
    f32 x, y, z;     ///< Position in world space.
    f32 u, v;        ///< Texture coordinates.
    f32 r, g, b, a;  ///< RGBA color.
};
