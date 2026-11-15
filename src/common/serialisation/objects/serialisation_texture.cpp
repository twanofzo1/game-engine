/**
 * @file serialisation_texture.cpp
 * @brief Serialise/deserialise the Texture type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a texture into a YAML node. */
YamlNode Serialiser::serialise_texture(const Texture* texture)
{
    YamlNode node;
    node["id"] = texture->id;
    node["name"] = texture->name;
    node["path"] = texture->path;
    node["size"] = Serialiser::serialise_vec2i(&texture->size);
    return node;
}

/** @brief Deserialise a texture from a YAML node. */
std::optional<Texture> Deserialiser::deserialise_texture(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("id")) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("path")) return std::nullopt;
    if (!node->operator[]("size")) return std::nullopt;

    YamlNode size_node = node->operator[]("size");
    std::optional<Vec2i> size = Deserialiser::deserialise_vec2i(&size_node);
    if (!size.has_value()) return std::nullopt;

    return Texture{
        .id = node->operator[]("id").as<TextureID>(),
        .name = node->operator[]("name").as<std::string>(),
        .path = node->operator[]("path").as<std::string>(),
        .size = size.value()
    };
}