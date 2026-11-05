/**
 * @file image.cpp
 * @brief Image loading via stb_image (PNG, JPG, BMP, GIF, TGA, HDR, ...).
 */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "resources/image.hpp"

namespace Engine {
namespace Image {

bool load_memory(Image* image, const u8* data, usize size)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    u8* pixels = stbi_load_from_memory(data, (int)size, &width, &height, &channels, 4);
    if (!pixels)
        return false;

    image->width = width;
    image->height = height;
    image->channels = 4;
    image->pixels = pixels;
    return true;
}

bool load(Image* image, const char* path)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    u8* pixels = stbi_load(path, &width, &height, &channels, 4);
    if (!pixels)
        return false;

    image->width = width;
    image->height = height;
    image->channels = 4;
    image->pixels = pixels;
    return true;
}

void destroy(Image* image)
{
    stbi_image_free(image->pixels);
    image->pixels = NULL;
    image->width = 0;
    image->height = 0;
    image->channels = 0;
}

} // namespace Image
} // namespace Engine
