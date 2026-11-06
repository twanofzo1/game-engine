/**
 * @file serialisation_vec2i.cpp
 * @brief Serialise/deserialise the Vec2i type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a 2D integer vector into a YAML node. */
Yaml::Node Serialiser::serialise_vec2i(const Vec2i* vec)
{
    Yaml::Node node;
    node["x"] = vec->x;
    node["y"] = vec->y;
    return node;
}

/** @brief Deserialise a 2D integer vector from a YAML node, or none if invalid. */
std::optional<Vec2i> Deserialiser::deserialise_vec2i(const Yaml::Node* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("x")) return std::nullopt;
    if (!node->operator[]("y")) return std::nullopt;

    return Vec2i::make({
        .x = node->operator[]("x").as<i32>(),
        .y = node->operator[]("y").as<i32>()
    });
}
