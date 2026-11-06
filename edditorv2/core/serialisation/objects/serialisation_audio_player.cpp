/**
 * @file serialisation_audio_player.cpp
 * @brief Serialise/deserialise the AudioPlayer type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an audio player into a YAML node. */
Yaml::Node Serialiser::serialise_audio_player(const AudioPlayer* audio_player)
{
    (void)audio_player;
    return Yaml::Node();
}

/** @brief Deserialise an audio player from a YAML node. */
std::optional<AudioPlayer> Deserialiser::deserialise_audio_player(const Yaml::Node* node)
{
    (void)node;
    return std::nullopt;
}