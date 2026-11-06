/**
 * @file serialisation_area.cpp
 * @brief Serialise/deserialise the Area type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an area into a YAML node. */
Yaml::Node Serialiser::serialise_area(const Area* area)
{
    (void)area;
    return Yaml::Node();
}

/** @brief Deserialise an area from a YAML node. */
std::optional<Area> Deserialiser::deserialise_area(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}