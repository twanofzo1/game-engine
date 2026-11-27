/**
 * @file serialisation_hurtbox.cpp
 * @brief Serialise/deserialise the Hurtbox type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a hurtbox into a YAML node. */
YamlNode Serialiser::serialise_hurtbox(const Hurtbox* hurtbox)
{
    YamlNode node;
    node["name"] = hurtbox->name;
    node["shape"] = Serialiser::serialise_shape(&hurtbox->shape);
    return node;
}

/** @brief Deserialise a hurtbox from a YAML node. */
std::optional<Hurtbox> Deserialiser::deserialise_hurtbox(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("shape")) return std::nullopt;

    Hurtbox hurtbox = {};
    hurtbox.name = node->operator[]("name").as<std::string>();
    const YamlNode shape_node = node->operator[]("shape");
    std::optional<Shape> shape = Deserialiser::deserialise_shape(&shape_node);
    if (!shape.has_value()) return std::nullopt;
    hurtbox.shape = shape.value();

    return hurtbox;
}