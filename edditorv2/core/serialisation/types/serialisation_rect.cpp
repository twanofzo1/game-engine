/**
 * @file serialisation_rect.cpp
 * @brief Serialise/deserialise the Rect type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a rectangle into a YAML node. */
Yaml::Node Serialiser::serialise_rect(const Rect* rect)
{
    Yaml::Node node;
    node["position"] = Serialiser::serialise_vec2(&rect->position);
    node["size"] = Serialiser::serialise_vec2(&rect->size);
    return node;
}

/** @brief Deserialise a rectangle from a YAML node, or none if invalid. */
std::optional<Rect> Deserialiser::deserialise_rect(const Yaml::Node* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("position")) return std::nullopt;
    if (!node->operator[]("size")) return std::nullopt;

    std::optional<Vec2> position = Deserialiser::deserialise_vec2(&node->operator[]("position"));
    if (!position.has_value()) return std::nullopt;

    std::optional<Vec2> size = Deserialiser::deserialise_vec2(&node->operator[]("size"));
    if (!size.has_value()) return std::nullopt;

    return Rect::make({
        .position = position.value(),
        .size     = size.value()
    });
}
