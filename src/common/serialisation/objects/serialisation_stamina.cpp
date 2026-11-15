/**
 * @file serialisation_stamina.cpp
 * @brief Serialise/deserialise the Stamina type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a stamina value into a YAML node. */
YamlNode Serialiser::serialise_stamina(const Stamina* stamina)
{
    YamlNode node;
    node["max"] = stamina->max;
    node["regen_rate"] = stamina->regen_rate;
    return node;
}

/** @brief Deserialise a stamina value from a YAML node, or none if invalid. */
std::optional<Stamina> Deserialiser::deserialise_stamina(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("max")) return std::nullopt;
    if (!node->operator[]("regen_rate")) return std::nullopt;

    return Stamina{
        .max = node->operator[]("max").as<u16>(),
        .regen_rate = node->operator[]("regen_rate").as<u16>()
    };
}
