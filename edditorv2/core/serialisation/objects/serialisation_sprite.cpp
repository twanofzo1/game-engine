/**
 * @file serialisation_sprite.cpp
 * @brief Serialise/deserialise the Sprite type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a sprite into a YAML node. */
Yaml::Node Serialiser::serialise_sprite(const Sprite* sprite)
{
    (void)sprite;
    return Yaml::Node();
}

/** @brief Deserialise a sprite from a YAML node. */
std::optional<Sprite> Deserialiser::deserialise_sprite(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}