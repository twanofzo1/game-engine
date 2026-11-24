/**
 * @file texture.hpp
 * @brief Declares a 2D GPU texture and its sampling settings.
 */
#pragma once

#include <optional>

#include "common/core/typedefs.hpp"


/// @brief Texture filtering mode.
enum Filter {
    FILTER_NEAREST, ///< Nearest-neighbor (crisp pixels).
    FILTER_LINEAR   ///< Bilinear (smooth).
};

/// @brief Texture wrap mode.
enum Wrap {
    WRAP_CLAMP,  ///< Clamp to edge (default).
    WRAP_REPEAT, ///< Tile the texture.
    WRAP_MIRROR  ///< Mirror-tile the texture.
};

/// @brief Texture sampling settings.
struct Settings {
    Filter min_filter = FILTER_LINEAR; ///< Minification filter.
    Filter mag_filter = FILTER_LINEAR; ///< Magnification filter.
    Wrap   wrap_s = WRAP_REPEAT;       ///< Horizontal (S) wrap mode.
    Wrap   wrap_t = WRAP_REPEAT;       ///< Vertical (T) wrap mode.
};

/// @brief Configuration for loading a texture from an image file.
struct TextureConfigPath {
    std::string path = "";  ///< Path to the texture file.
    Settings settings = {}; ///< Sampling settings to use when uploading.
};

/// @brief Configuration for loading a texture from raw pixel data.
struct TextureConfigData {
    const u8* data = nullptr; ///< RGBA8 pixel data (row-major, top row first).
    i32 width = 0;            ///< Width in pixels.
    i32 height = 0;           ///< Height in pixels.
    Settings settings = {};   ///< Sampling settings to use when uploading.
};

/// @brief A 2D GPU texture.
struct Texture {
    u32      id = (u32)-1;  ///< OpenGL texture name.
    i32      width = 0;     ///< Width in pixels.
    i32      height = 0;    ///< Height in pixels.
    Settings settings = {}; ///< Current sampling settings.

    static std::optional<Texture> load(TextureConfigPath config);

    static std::optional<Texture> load(TextureConfigData config);

    void bind(i32 unit = 0) const;

    static void unbind(i32 unit = 0);

    void destroy();

    void set_settings(const Settings* settings);
};



