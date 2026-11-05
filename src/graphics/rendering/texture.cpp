/**
 * @file texture.cpp
 * @brief 2D GPU texture implementation.
 */
#include "graphics/rendering/texture.hpp"

#include <glad/glad.h>

namespace Engine {
namespace Texture {

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

Settings settings_default()
{
    Settings settings;
    settings.min_filter = FILTER_LINEAR;
    settings.mag_filter = FILTER_LINEAR;
    settings.wrap_s = WRAP_CLAMP;
    settings.wrap_t = WRAP_CLAMP;
    return settings;
}

/** @brief Apply the given settings to the currently bound texture. */
static void apply_settings(const Settings* settings)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter(settings->min_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter(settings->mag_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap(settings->wrap_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap(settings->wrap_t));
}

bool create(Texture* texture, const u8* rgba, i32 width, i32 height,
            const Settings* settings)
{
    texture->id = 0;
    texture->width = width;
    texture->height = height;
    texture->settings = settings ? *settings : settings_default();

    glGenTextures(1, &texture->id);
    if (texture->id == 0)
        return false;

    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    apply_settings(&texture->settings);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void set_settings(Texture* texture, const Settings* settings)
{
    if (!settings)
        return;

    texture->settings = *settings;
    glBindTexture(GL_TEXTURE_2D, texture->id);
    apply_settings(&texture->settings);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void destroy(Texture* texture)
{
    if (texture->id)
    {
        glDeleteTextures(1, &texture->id);
        texture->id = 0;
    }
    texture->width = 0;
    texture->height = 0;
}

void bind(const Texture* texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

} // namespace Texture
} // namespace Engine
