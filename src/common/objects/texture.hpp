/**
 * @file texture.hpp
 * @brief Texture type: an in-memory image with a file path.
 */
#pragma once
#include "common/core/types.hpp"

using TextureID = u32;

/** @brief An in-memory image loaded from disk, ready for upload to the GPU. */
struct Texture {
    TextureID id;      ///< OpenGL texture ID.
    std::string name;   ///< Unique identifier.
    std::string path;   ///< Filesystem path of the source image.
    Vec2i size;         ///< Pixel dimensions.
};