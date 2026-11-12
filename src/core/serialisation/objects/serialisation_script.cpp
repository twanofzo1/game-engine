/**
 * @file serialisation_script.cpp
 * @brief Serialise/deserialise the Script type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a script into a YAML node. */
YamlNode Serialiser::serialise_script(const Script* script)
{
    YamlNode node;
    node["id"] = script->id;
    node["name"] = script->name;
    node["path"] = script->path;
    return node;
}

/** @brief Deserialise a script from a YAML node, or none if invalid. */
std::optional<Script> Deserialiser::deserialise_script(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("id")) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("path")) return std::nullopt;

    return Script{
        .id = node->operator[]("id").as<ScriptID>(),
        .name = node->operator[]("name").as<std::string>(),
        .path = node->operator[]("path").as<std::string>()
    };
}
