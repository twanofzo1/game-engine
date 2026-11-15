/**
 * @file texture.hpp
 * @brief TextureAsset type: an in-memory image with a file path and size.
 */
#pragma once
#include "common/core/types.hpp"

using TextureID = u32;

/** @brief An in-memory image asset loaded from disk, ready for upload to the GPU. */
struct TextureAsset {
    TextureID id;      ///< OpenGL texture ID.
    std::string name;   ///< Unique identifier.
    std::string path;   ///< Filesystem path of the source image.
    Vec2i size;         ///< Pixel dimensions.
};