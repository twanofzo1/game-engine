/**
 * @file serialisation_sprite.cpp
 * @brief Serialise/deserialise the Sprite type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a sprite into a YAML node. */
YamlNode Serialiser::serialise_sprite(const Sprite* sprite)
{
    YamlNode node;
    node["id"] = sprite->id;
    node["name"] = sprite->name;
    node["texture"] = sprite->texture;
    node["size"] = Serialiser::serialise_vec2(&sprite->size);
    node["h_frames"] = sprite->h_frames;
    node["v_frames"] = sprite->v_frames;
    return node;
}

/** @brief Deserialise a sprite from a YAML node. */
std::optional<Sprite> Deserialiser::deserialise_sprite(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("id")) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("texture")) return std::nullopt;
    if (!node->operator[]("size")) return std::nullopt;
    if (!node->operator[]("h_frames")) return std::nullopt;
    if (!node->operator[]("v_frames")) return std::nullopt;

    YamlNode size_node = node->operator[]("size");
    std::optional<Vec2> size = Deserialiser::deserialise_vec2(&size_node);
    if (!size.has_value()) return std::nullopt;

    return Sprite{
        .id = node->operator[]("id").as<SpriteID>(),
        .name = node->operator[]("name").as<std::string>(),
        .texture = node->operator[]("texture").as<TextureID>(),
        .size = size.value(),
        .h_frames = node->operator[]("h_frames").as<u16>(),
        .v_frames = node->operator[]("v_frames").as<u16>(),
        .current_frame = 0
    };
}