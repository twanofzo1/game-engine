/**
 * @file texture.cpp
 * @brief YAML serialization for a texture asset description.
 */
#include "serialization/texture_yaml.hpp"

#include <string.h>

namespace Yaml {

bool load_texture(const Node* node, TextureConfig* out)
{
    if (out == NULL)
        return false;

    out->path[0] = '\0';
    out->filter  = 1;
    out->repeat  = 0;
    out->size    = Engine::Vec2::make(0.0f, 0.0f);

    if (node == NULL || !node->IsMap())
        return false;

    std::string path = as_string((*node)["path"], "");
    strncpy(out->path, path.c_str(), TEXTURE_PATH_MAX - 1);
    out->path[TEXTURE_PATH_MAX - 1] = '\0';

    out->filter = as_int((*node)["filter"], out->filter);
    out->repeat = as_int((*node)["repeat"], out->repeat);
    out->size   = as_vec2((*node)["size"], out->size);

    return true;
}

Node make_texture(const TextureConfig* texture)
{
    Node node;

    node["path"]   = std::string(texture->path);
    node["filter"] = texture->filter;
    node["repeat"] = texture->repeat;
    node["size"]   = make_vec2(texture->size);

    return node;
}

} // namespace Yaml
