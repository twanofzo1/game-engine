/**
 * @file serialisation_color.cpp
 * @brief Serialise/deserialise the Color type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a color into a YAML node. */
Yaml::Node Serialiser::serialise_color(const Color* color)
{
    Yaml::Node node;
    node["r"] = color->r;
    node["g"] = color->g;
    node["b"] = color->b;
    node["a"] = color->a;
    return node;
}

/** @brief Deserialise a color from a YAML node, or none if invalid. */
std::optional<Color> Deserialiser::deserialise_color(const Yaml::Node* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("r")) return std::nullopt;
    if (!node->operator[]("g")) return std::nullopt;
    if (!node->operator[]("b")) return std::nullopt;
    if (!node->operator[]("a")) return std::nullopt;

    return Color::make({
        .r = node->operator[]("r").as<f32>(),
        .g = node->operator[]("g").as<f32>(),
        .b = node->operator[]("b").as<f32>(),
        .a = node->operator[]("a").as<f32>()
    });
}
