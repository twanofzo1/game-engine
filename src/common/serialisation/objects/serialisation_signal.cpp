/**
 * @file serialisation_signal.cpp
 * @brief Serialise/deserialise the Signal type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a signal into a YAML node. */
YamlNode Serialiser::serialise_signal(const Signal* signal)
{
    YamlNode node;
    node["name"] = signal->name;
    node["scripts"] = signal->scripts;
    return node;
}

/** @brief Deserialise a signal from a YAML node, or none if invalid. */
std::optional<Signal> Deserialiser::deserialise_signal(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("scripts")) return std::nullopt;

    return Signal{
        .name = node->operator[]("name").as<std::string>(),
        .scripts = node->operator[]("scripts").as<std::vector<ScriptID>>()
    };
}
