/**
 * @file serialisation_vec2.cpp
 * @brief Serialise/deserialise the Vec2 type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a 2D float vector into a YAML node. */
Yaml::Node Serialiser::serialise_vec2(const Vec2* vec)
{
    Yaml::Node node;
    node["x"] = vec->x;
    node["y"] = vec->y;
    return node;
}

/** @brief Deserialise a 2D float vector from a YAML node, or none if invalid. */
std::optional<Vec2> Deserialiser::deserialise_vec2(const Yaml::Node* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("x")) return std::nullopt;
    if (!node->operator[]("y")) return std::nullopt;

    return Vec2::make({
        .x = node->operator[]("x").as<f32>(),
        .y = node->operator[]("y").as<f32>()
    });
}
