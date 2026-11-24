/**
 * @file texture.cpp
 * @brief 2D GPU texture implementation.
 */
#include "common/graphics/texture/texture.hpp"
#include "common/log/log.hpp"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


/// @brief Maps a Filter to its OpenGL constant.
/// @param filter The filter.
/// @return The GL filter constant.
static GLint gl_filter(const Filter filter)
{
    return filter == FILTER_NEAREST ? GL_NEAREST : GL_LINEAR;
}

/// @brief Maps a Wrap to its OpenGL constant.
/// @param wrap The wrap mode.
/// @return The GL wrap constant.
static GLint gl_wrap(const Wrap wrap)
{
    switch (wrap)
    {
        case WRAP_REPEAT: return GL_REPEAT;
        case WRAP_MIRROR: return GL_MIRRORED_REPEAT;
        default:          return GL_CLAMP_TO_EDGE;
    }
}

/// @brief Applies the given settings to the currently bound texture.
/// @param settings The sampling settings.
static void apply_settings(const Settings* const settings)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter(settings->min_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter(settings->mag_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap(settings->wrap_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap(settings->wrap_t));
}

/// @brief Uploads RGBA8 pixel data to a freshly created texture.
/// @param rgba The pixel data (may be null for an empty texture).
/// @param width The width in pixels.
/// @param height The height in pixels.
/// @param settings The sampling settings.
/// @return The texture, or nullopt when texture creation fails.
static std::optional<Texture> create_texture(const u8* const rgba, const i32 width, const i32 height,
                                             const Settings* const settings)
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

/// @brief Loads a texture from an image file on disk.
/// @param config The file path and sampling settings.
/// @return The texture, or nullopt when loading fails.
std::optional<Texture> Texture::load(const TextureConfigPath config)
{
    log_debug(LOG_SECTION_TEXTURE, "loading '%s'", config.path.c_str());

    i32 width = 0;
    i32 height = 0;
    i32 channels = 0;

    u8* const pixels = stbi_load(config.path.c_str(), &width, &height, &channels, 4);
    if (!pixels) {
        log_debug(LOG_SECTION_TEXTURE, "stbi_load failed for '%s'", config.path.c_str());
        return std::nullopt;
    }

    const std::optional<Texture> texture = create_texture(pixels, width, height, &config.settings);
    stbi_image_free(pixels);
    if (texture.has_value()) {
        log_debug(LOG_SECTION_TEXTURE, "loaded %dx%d (id=%u)", texture->width, texture->height, texture->id);
    }
    return texture;
}

/// @brief Loads a texture from raw RGBA8 pixel data in memory.
/// @param config The pixel data and sampling settings.
/// @return The texture, or nullopt when creation fails.
std::optional<Texture> Texture::load(const TextureConfigData config)
{
    log_debug(LOG_SECTION_TEXTURE, "loading from data %dx%d", config.width, config.height);
    return create_texture(config.data, config.width, config.height, &config.settings);
}

/// @brief Binds this texture to the given texture unit.
/// @param unit The texture unit (0 by default).
void Texture::bind(const i32 unit) const
{
    log_debug(LOG_SECTION_TEXTURE, "bind id=%u unit=%d", this->id, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, this->id);
}

/// @brief Unbinds any texture from the given texture unit.
/// @param unit The texture unit (0 by default).
void Texture::unbind(const i32 unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/// @brief Destroys this texture and resets its state.
void Texture::destroy()
{
    log_debug(LOG_SECTION_TEXTURE, "destroy id=%u", this->id);
    if (this->id)
    {
        glDeleteTextures(1, &this->id);
        this->id = 0;
    }
    this->width = 0;
    this->height = 0;
}

/// @brief Changes this texture's sampling settings.
/// @param settings The new sampling settings.
void Texture::set_settings(const Settings* const settings)
{
    if (!settings)
        return;

    this->settings = *settings;
    glBindTexture(GL_TEXTURE_2D, this->id);
    apply_settings(&this->settings);
    glBindTexture(GL_TEXTURE_2D, 0);
}


