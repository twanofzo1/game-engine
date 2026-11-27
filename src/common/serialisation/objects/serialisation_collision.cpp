/**
 * @file serialisation_collision.cpp
 * @brief Serialise/deserialise the Collision type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a collision shape into a YAML node. */
YamlNode Serialiser::serialise_collision(const Collision* collision)
{
    YamlNode node;
    node["name"] = collision->name;
    node["shape"] = Serialiser::serialise_shape(&collision->shape);
    return node;
}

/** @brief Deserialise a collision shape from a YAML node. */
std::optional<Collision> Deserialiser::deserialise_collision(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("shape")) return std::nullopt;

    Collision collision = {};
    const YamlNode name_node = node->operator[]("name");
    collision.name = name_node.as<std::string>();
    const YamlNode shape_node = node->operator[]("shape");
    std::optional<Shape> shape = Deserialiser::deserialise_shape(&shape_node);
    if (!shape.has_value()) return std::nullopt;
    collision.shape = shape.value();

    return collision;
}