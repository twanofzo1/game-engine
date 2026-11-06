/**
 * @file serialisation_hitbox.cpp
 * @brief Serialise/deserialise the Hitbox type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a hitbox into a YAML node. */
Yaml::Node Serialiser::serialise_hitbox(const Hitbox* hitbox)
{
    (void)hitbox;
    return Yaml::Node();
}

/** @brief Deserialise a hitbox from a YAML node. */
std::optional<Hitbox> Deserialiser::deserialise_hitbox(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}