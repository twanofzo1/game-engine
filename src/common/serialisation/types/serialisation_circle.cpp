/**
 * @file serialisation_circle.cpp
 * @brief Serialise/deserialise the Circle type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a circle into a YAML node. */
YamlNode Serialiser::serialise_circle(const Circle* circle)
{
    YamlNode node;
    node["position"] = Serialiser::serialise_vec2(&circle->position);
    node["radius"] = circle->radius;
    return node;
}

/** @brief Deserialise a circle from a YAML node, or none if invalid. */
std::optional<Circle> Deserialiser::deserialise_circle(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("position")) return std::nullopt;
    if (!node->operator[]("radius")) return std::nullopt;

    YamlNode position_node = node->operator[]("position");
    std::optional<Vec2> position = Deserialiser::deserialise_vec2(&position_node);
    if (!position.has_value()) return std::nullopt;

    f32 radius = node->operator[]("radius").as<f32>();

    return Circle::create({
        .position = position.value(),
        .radius   = radius
    });
}
