/**
 * @file serialisation_animation_player.cpp
 * @brief Serialise/deserialise the AnimationPlayer type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an animation player into a YAML node. */
Yaml::Node Serialiser::serialise_animation_player(const AnimationPlayer* animation_player)
{
    (void)animation_player;
    return Yaml::Node();
}

/** @brief Deserialise an animation player from a YAML node. */
std::optional<AnimationPlayer> Deserialiser::deserialise_animation_player(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}