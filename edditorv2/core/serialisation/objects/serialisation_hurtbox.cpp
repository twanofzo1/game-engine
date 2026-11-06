/**
 * @file serialisation_hurtbox.cpp
 * @brief Serialise/deserialise the Hurtbox type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a hurtbox into a YAML node. */
Yaml::Node Serialiser::serialise_hurtbox(const Hurtbox* hurtbox)
{
    (void)hurtbox;
    return Yaml::Node();
}

/** @brief Deserialise a hurtbox from a YAML node. */
std::optional<Hurtbox> Deserialiser::deserialise_hurtbox(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}