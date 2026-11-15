/**
 * @file texture.hpp
 * @brief Declares a 2D GPU texture and its sampling settings.
 */
#pragma once

#include <optional>

#include "common/core/typedefs.hpp"


enum Filter {
    FILTER_NEAREST, ///< Nearest-neighbor (crisp pixels).
    FILTER_LINEAR   ///< Bilinear (smooth).
};

enum Wrap {
    WRAP_CLAMP,  ///< Clamp to edge (default).
    WRAP_REPEAT, ///< Tile the texture.
    WRAP_MIRROR  ///< Mirror-tile the texture.
};

struct Settings {
    Filter min_filter = FILTER_LINEAR; ///< Minification filter.
    Filter mag_filter = FILTER_LINEAR; ///< Magnification filter.
    Wrap   wrap_s = WRAP_REPEAT;     ///< Horizontal (S) wrap mode.
    Wrap   wrap_t = WRAP_REPEAT;     ///< Vertical (T) wrap mode.
};

struct TextureConfigPath {
    std::string path = "";      ///< Path to the texture file.
    Settings settings = {};     ///< Sampling settings to use when uploading.
};

struct TextureConfigData {
    const u8* data = nullptr;   ///< RGBA8 pixel data (row-major, top row first).
    i32 width = 0;              ///< Width in pixels.
    i32 height = 0;             ///< Height in pixels.
    Settings settings = {};     ///< Sampling settings to use when uploading.
};


struct Texture {
    u32      id = (u32)-1;  ///< OpenGL texture name.
    i32      width = 0;     ///< Width in pixels.
    i32      height = 0;    ///< Height in pixels.
    Settings settings = {}; ///< Current sampling settings.

    /** @brief Load a texture from an image file, or nullopt on failure. */
    static std::optional<Texture> load(TextureConfigPath config);

    /** @brief Load a texture from raw RGBA8 pixel data, or nullopt on failure. */
    static std::optional<Texture> load(TextureConfigData config);

    /** @brief Bind the texture to a specific texture unit (0 by default). */
    static void bind(const Texture* texture, i32 unit = 0);

    /** @brief Unbind the texture from a specific texture unit (0 by default). */
    static void unbind(i32 unit = 0);

    /** @brief Destroy the texture and reset its state. */
    static void destroy(Texture* texture);

    /** @brief Change the texture's sampling settings. */
    static void set_settings(Texture* texture, const Settings* settings);
};



