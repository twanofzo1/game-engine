/**
 * @file image.hpp
 * @brief Declares a decoded RGBA8 image resource (stb_image backend).
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Image {

/** A decoded RGBA8 image. Pixels can be uploaded with `Texture::create`. */
struct Image {
    i32 width;    ///< Width in pixels.
    i32 height;   ///< Height in pixels.
    i32 channels; ///< Number of color channels (4).
    u8* pixels;   ///< RGBA8 pixels, row-major (top row first).
};

/** @brief Free the pixel buffer owned by `image`. */
void destroy(Image* image);

/**
 * @brief Decode an image from memory (PNG, JPG, BMP, GIF, TGA, HDR, ...).
 * @param image  Destination image.
 * @param data   Encoded file bytes.
 * @param size   Number of encoded bytes.
 * @return True on success.
 */
bool load_memory(Image* image, const u8* data, usize size);

/**
 * @brief Load and decode an image file (format detected from content).
 * @return True on success.
 */
bool load(Image* image, const char* path);

} // namespace Image
} // namespace Engine
