/**
 * @file serialisation_rect.cpp
 * @brief Serialise/deserialise the Rect type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a rectangle into a YAML node. */
YamlNode Serialiser::serialise_rect(const Rect* rect)
{
    YamlNode node;
    node["position"] = Serialiser::serialise_vec2(&rect->position);
    node["size"] = Serialiser::serialise_vec2(&rect->size);
    return node;
}

/** @brief Deserialise a rectangle from a YAML node, or none if invalid. */
std::optional<Rect> Deserialiser::deserialise_rect(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("position")) return std::nullopt;
    if (!node->operator[]("size")) return std::nullopt;

    YamlNode position_node = node->operator[]("position");
    std::optional<Vec2> position = Deserialiser::deserialise_vec2(&position_node);
    if (!position.has_value()) return std::nullopt;

    YamlNode size_node = node->operator[]("size");
    std::optional<Vec2> size = Deserialiser::deserialise_vec2(&size_node);
    if (!size.has_value()) return std::nullopt;

    return Rect::create({
        .position = position.value(),
        .size     = size.value()
    });
}
