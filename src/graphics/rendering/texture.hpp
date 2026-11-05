/**
 * @file texture.hpp
 * @brief Declares a 2D GPU texture and its sampling settings.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Texture {

/** Minification / magnification filter. */
enum Filter {
    FILTER_NEAREST, ///< Nearest-neighbor (crisp pixels).
    FILTER_LINEAR   ///< Bilinear (smooth).
};

/** Texture coordinate wrap mode. */
enum Wrap {
    WRAP_CLAMP,  ///< Clamp to edge (default).
    WRAP_REPEAT, ///< Tile the texture.
    WRAP_MIRROR  ///< Mirror-tile the texture.
};

/** Sampling settings applied to a texture. */
struct Settings {
    Filter min_filter; ///< Filter when the texture is minified.
    Filter mag_filter; ///< Filter when the texture is magnified.
    Wrap   wrap_s;     ///< Horizontal (S) wrap mode.
    Wrap   wrap_t;     ///< Vertical (T) wrap mode.
};

/** @brief Default settings: linear filtering, clamp to edge. */
Settings settings_default();

/** A 2D RGBA8 texture uploaded to the GPU. */
struct Texture {
    u32      id;       ///< OpenGL texture name.
    i32      width;    ///< Width in pixels.
    i32      height;   ///< Height in pixels.
    Settings settings; ///< Current sampling settings.
};

/**
 * @brief Upload RGBA8 pixel data to a new texture.
 * @param texture  Destination texture.
 * @param rgba     RGBA8 pixels (row-major, top row first).
 * @param width    Width in pixels.
 * @param height   Height in pixels.
 * @param settings Sampling settings, or NULL for the defaults.
 * @return True on success.
 */
bool create(Texture* texture, const u8* rgba, i32 width, i32 height,
            const Settings* settings);

/** @brief Change the sampling settings of an existing texture. */
void set_settings(Texture* texture, const Settings* settings);

/** @brief Delete the texture from the GPU. */
void destroy(Texture* texture);

/** @brief Bind the texture for drawing. */
void bind(const Texture* texture);

} // namespace Texture
} // namespace Engine
