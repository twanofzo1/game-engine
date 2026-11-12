/**
 * @file serialisation_area.cpp
 * @brief Serialise/deserialise the Area type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an area into a YAML node. */
YamlNode Serialiser::serialise_area(const Area* area)
{
    YamlNode node;
    node["name"] = area->name;
    node["shape"] = Serialiser::serialise_shape(&area->shape);
    node["continuous"] = area->continuous;
    return node;
}

/** @brief Deserialise an area from a YAML node. */
std::optional<Area> Deserialiser::deserialise_area(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("shape")) return std::nullopt;
    if (!node->operator[]("continuous")) return std::nullopt;

    YamlNode shape_node = node->operator[]("shape");
    std::optional<Shape> shape = Deserialiser::deserialise_shape(&shape_node);
    if (!shape.has_value()) return std::nullopt;

    return Area{
        .name = node->operator[]("name").as<std::string>(),
        .shape = shape.value(),
        .continuous = node->operator[]("continuous").as<bool>()
    };
}