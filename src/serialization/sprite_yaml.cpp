/**
 * @file sprite.cpp
 * @brief YAML serialization for a sprite description.
 */
#include "serialization/sprite_yaml.hpp"

#include <string.h>

namespace Yaml {

bool load_sprite(const Node* node, SpriteConfig* out)
{
    if (out == NULL)
        return false;

    out->name[0]     = '\0';
    out->texture_id = 0;
    out->h_frames   = 1;
    out->v_frames   = 1;
    out->pos        = Engine::Vec2::make(0.0f, 0.0f);
    out->scale      = 1.0f;

    if (node == NULL || !node->IsMap())
        return false;

    std::string name = as_string((*node)["name"], "");
    strncpy(out->name, name.c_str(), SPRITE_NAME_MAX - 1);
    out->name[SPRITE_NAME_MAX - 1] = '\0';

    out->texture_id = as_int((*node)["texture_id"], out->texture_id);
    out->h_frames   = as_int((*node)["h_frames"], out->h_frames);
    out->v_frames   = as_int((*node)["v_frames"], out->v_frames);
    if (out->h_frames < 1) out->h_frames = 1;
    if (out->v_frames < 1) out->v_frames = 1;
    out->pos        = as_vec2((*node)["pos"], out->pos);
    out->scale      = as_float((*node)["scale"], out->scale);

    return true;
}

Node make_sprite(const SpriteConfig* sprite)
{
    Node node;

    if (sprite->name[0] != '\0')
        node["name"] = std::string(sprite->name);
    node["texture_id"] = sprite->texture_id;
    node["h_frames"]   = sprite->h_frames;
    node["v_frames"]   = sprite->v_frames;
    node["pos"]        = make_vec2(sprite->pos);
    node["scale"]      = sprite->scale;

    return node;
}

} // namespace Yaml
