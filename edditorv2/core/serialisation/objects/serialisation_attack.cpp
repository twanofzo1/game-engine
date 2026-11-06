/**
 * @file serialisation_attack.cpp
 * @brief Serialise/deserialise the Attack type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an attack into a YAML node. */
Yaml::Node Serialiser::serialise_attack(const Attack* attack)
{
    (void)attack;
    return Yaml::Node();
}

/** @brief Deserialise an attack from a YAML node. */
std::optional<Attack> Deserialiser::deserialise_attack(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}