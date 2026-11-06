/**
 * @file serialisation_circle.cpp
 * @brief Serialise/deserialise the Circle type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a circle into a YAML node. */
Yaml::Node Serialiser::serialise_circle(const Circle* circle)
{
    Yaml::Node node;
    node["position"] = Serialiser::serialise_vec2(&circle->position);
    node["radius"] = circle->radius;
    return node;
}

/** @brief Deserialise a circle from a YAML node, or none if invalid. */
std::optional<Circle> Deserialiser::deserialise_circle(const Yaml::Node* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("position")) return std::nullopt;
    if (!node->operator[]("radius")) return std::nullopt;

    std::optional<Vec2> position = Deserialiser::deserialise_vec2(&node->operator[]("position"));
    if (!position.has_value()) return std::nullopt;

    f32 radius = node->operator[]("radius").as<f32>();

    return Circle::make({
        .position = position.value(),
        .radius   = radius
    });
}
