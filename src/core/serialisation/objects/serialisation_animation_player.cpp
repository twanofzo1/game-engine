/**
 * @file serialisation_animation_player.cpp
 * @brief Serialise/deserialise the AnimationPlayer type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an animation player into a YAML node. */
YamlNode Serialiser::serialise_animation_player(const AnimationPlayer* animation_player)
{
    (void)animation_player;
    return YamlNode();
}

/** @brief Deserialise an animation player from a YAML node. */
std::optional<AnimationPlayer> Deserialiser::deserialise_animation_player(const YamlNode* node)
{
    (void)node;
    return std::nullopt;
}