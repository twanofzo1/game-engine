/**
 * @file serialisation_collision.cpp
 * @brief Serialise/deserialise the Collision type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a collision shape into a YAML node. */
Yaml::Node Serialiser::serialise_collision(const Collision* collision)
{
    (void)collision;
    return Yaml::Node();
}

/** @brief Deserialise a collision shape from a YAML node. */
std::optional<Collision> Deserialiser::deserialise_collision(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}