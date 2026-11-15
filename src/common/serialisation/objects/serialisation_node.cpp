/**
 * @file serialisation_node.cpp
 * @brief Serialise/deserialise the Node type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a node into a YAML node. */
YamlNode Serialiser::serialise_node(const Node* node)
{
    YamlNode result;
    result["script"] = node->script;
    result["children"] = node->children;
    return result;
}

/** @brief Deserialise a node from a YAML node, or none if invalid. */
std::optional<Node> Deserialiser::deserialise_node(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("script")) return std::nullopt;
    if (!node->operator[]("children")) return std::nullopt;

    return Node{
        .script = node->operator[]("script").as<ScriptID>(),
        .children = node->operator[]("children").as<std::vector<NodeID>>()
    };
}
