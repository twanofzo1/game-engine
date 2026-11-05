/**
 * @file vertex.hpp
 * @brief Vertex layout shared by the renderer and its shaders.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Render {

/** One vertex: position, texture coordinate and RGBA color. */
struct Vertex {
    f32 x, y, z;     ///< Position in world space.
    f32 u, v;        ///< Texture coordinates (reserved for future textures).
    f32 r, g, b, a;  ///< RGBA color.
};

} // namespace Render
} // namespace Engine
