/**
 * @file texture.cpp
 * @brief 2D GPU texture implementation.
 */
#include "common/graphics/texture/texture.hpp"
#include "common/log/log.hpp"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


/** @brief Map a Filter to its OpenGL constant. */
static GLint gl_filter(Filter filter)
{
    return filter == FILTER_NEAREST ? GL_NEAREST : GL_LINEAR;
}

/** @brief Map a Wrap to its OpenGL constant. */
static GLint gl_wrap(Wrap wrap)
{
    switch (wrap)
    {
        case WRAP_REPEAT: return GL_REPEAT;
        case WRAP_MIRROR: return GL_MIRRORED_REPEAT;
        default:          return GL_CLAMP_TO_EDGE;
    }
}

/** @brief Apply the given settings to the currently bound texture. */
static void apply_settings(const Settings* settings)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter(settings->min_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter(settings->mag_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap(settings->wrap_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap(settings->wrap_t));
}

/** @brief Upload RGBA8 pixel data to a freshly created texture. */
static std::optional<Texture> create_texture(const u8* rgba, i32 width, i32 height,
                                             const Settings* settings)
{
    Texture texture;
    texture.id = 0;
    texture.width = width;
    texture.height = height;
    texture.settings = settings ? *settings : Settings{};

    glGenTextures(1, &texture.id);
    if (texture.id == 0)
        return std::nullopt;

    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    apply_settings(&texture.settings);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

/** @brief Load a texture from an image file on disk. */
std::optional<Texture> Texture::load(TextureConfigPath config)
{
    log_debug(LOG_SECTION_TEXTURE, "loading '%s'", config.path.c_str());

    i32 width = 0;
    i32 height = 0;
    i32 channels = 0;

    u8* pixels = stbi_load(config.path.c_str(), &width, &height, &channels, 4);
    if (!pixels) {
        log_debug(LOG_SECTION_TEXTURE, "stbi_load failed for '%s'", config.path.c_str());
        return std::nullopt;
    }

    std::optional<Texture> texture = create_texture(pixels, width, height, &config.settings);
    stbi_image_free(pixels);
    if (texture.has_value()) {
        log_debug(LOG_SECTION_TEXTURE, "loaded %dx%d (id=%u)", texture->width, texture->height, texture->id);
    }
    return texture;
}

/** @brief Load a texture from raw RGBA8 pixel data in memory. */
std::optional<Texture> Texture::load(TextureConfigData config)
{
    log_debug(LOG_SECTION_TEXTURE, "loading from data %dx%d", config.width, config.height);
    return create_texture(config.data, config.width, config.height, &config.settings);
}

/** @brief Bind the texture to the given texture unit. */
void Texture::bind(const Texture* texture, i32 unit)
{
    log_debug(LOG_SECTION_TEXTURE, "bind id=%u unit=%d", texture->id, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

/** @brief Unbind any texture from the given texture unit. */
void Texture::unbind(i32 unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/** @brief Destroy the texture and reset its state. */
void Texture::destroy(Texture* texture)
{
    log_debug(LOG_SECTION_TEXTURE, "destroy id=%u", texture->id);
    if (texture->id)
    {
        glDeleteTextures(1, &texture->id);
        texture->id = 0;
    }
    texture->width = 0;
    texture->height = 0;
}

/** @brief Change the texture's sampling settings. */
void Texture::set_settings(Texture* texture, const Settings* settings)
{
    if (!settings)
        return;

    texture->settings = *settings;
    glBindTexture(GL_TEXTURE_2D, texture->id);
    apply_settings(&texture->settings);
    glBindTexture(GL_TEXTURE_2D, 0);
}


