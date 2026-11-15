/**
 * @file serialisation_hitbox.cpp
 * @brief Serialise/deserialise the Hitbox type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a hitbox into a YAML node. */
YamlNode Serialiser::serialise_hitbox(const Hitbox* hitbox)
{
    YamlNode node;
    node["name"] = hitbox->name;
    node["shape"] = Serialiser::serialise_shape(&hitbox->shape);
    return node;
}

/** @brief Deserialise a hitbox from a YAML node. */
std::optional<Hitbox> Deserialiser::deserialise_hitbox(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("shape")) return std::nullopt;

    YamlNode shape_node = node->operator[]("shape");
    std::optional<Shape> shape = Deserialiser::deserialise_shape(&shape_node);
    if (!shape.has_value()) return std::nullopt;

    return Hitbox{
        .name = node->operator[]("name").as<std::string>(),
        .shape = shape.value()
    };
}