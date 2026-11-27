/**
 * @file renderer.hpp
 * @brief Batched quad renderer with painter's-algorithm z-ordering.
 */
#pragma once

#include <vector>

#include "common/core/typedefs.hpp"
#include "common/core/types/color.hpp"
#include "common/core/types/vec2.hpp"
#include "common/graphics/rendering/vertex.hpp"

/** Maximum number of quads buffered before a flush is forced. */
constexpr u32 RENDERER_MAX_QUADS = 4096;

/// @brief Configuration used to submit one quad to the batch.
struct QuadConfig {
    Vec2  position = Vec2::create({.x = 0.0f, .y = 0.0f}); ///< Bottom-left position in world units.
    Vec2  size = Vec2::create({.x = 1.0f, .y = 1.0f});     ///< Size in world units.
    Color color = Color::from_8_bit(255, 255, 255, 255);   ///< Tint color (RGBA).
    u32   texture = 0;     ///< GPU texture name; 0 is the solid white texture.
    u32   shader = 0;      ///< Shader program name; 0 is the default quad shader.
    f32   z = 0.0f;        ///< Draw order; higher z draws on top.
    Vec2  uv_min = Vec2::create({.x = 0.0f, .y = 0.0f});  ///< Top-left UV of the source rect.
    Vec2  uv_max = Vec2::create({.x = 1.0f, .y = 1.0f});  ///< Bottom-right UV of the source rect.
};

/// @brief One buffered quad, holding its final draw ordering.
struct Quad {
    u32 shader;                       ///< Shader program name.
    u32 texture;                      ///< GPU texture name.
    f32 z;                            ///< Draw order.
    Vertex vertices[VERTICES_PER_QUAD]; ///< Four corners in local order.
};

/// @brief A multi-shader, multi-texture batched quad renderer.
struct Renderer {
    std::vector<Quad> quads; ///< Buffered quads awaiting `end`.

    static Renderer create(u32 default_shader);

    void begin();
    void push_quad(const QuadConfig config);
    void end();

private:
    u32 default_shader_; ///< Program used when a quad omits `shader`.
    bool active_;        ///< Whether a batch is currently being accumulated.

};
