/**
 * @file serialisation_texture.cpp
 * @brief Serialise/deserialise the Texture type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a texture into a YAML node. */
YamlNode Serialiser::serialise_texture(const Texture* texture)
{
    (void)texture;
    return YamlNode();
}

/** @brief Deserialise a texture from a YAML node. */
std::optional<Texture> Deserialiser::deserialise_texture(const YamlNode* node)
{
    (void)node;
    return std::nullopt;
}