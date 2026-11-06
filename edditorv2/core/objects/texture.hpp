/**
 * @file texture.hpp
 * @brief Texture type: an in-memory image with a file path.
 */
#pragma once
#include "core/types.hpp"

/** @brief An in-memory image loaded from disk, ready for upload to the GPU. */
struct Texture {
    std::string name;   ///< Unique identifier.
    std::string path;   ///< Filesystem path of the source image.
    Vec2i size;         ///< Pixel dimensions.
    u8* data;           ///< Raw pixel buffer.
};