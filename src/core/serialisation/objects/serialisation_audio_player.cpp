/**
 * @file serialisation_audio_player.cpp
 * @brief Serialise/deserialise the AudioPlayer type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an audio player into a YAML node. */
YamlNode Serialiser::serialise_audio_player(const AudioPlayer* audio_player)
{
    YamlNode node;
    node["playlist"] = audio_player->playlist;
    node["volume"] = audio_player->volume;
    node["loop"] = audio_player->loop;
    return node;
}

/** @brief Deserialise an audio player from a YAML node. */
std::optional<AudioPlayer> Deserialiser::deserialise_audio_player(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("playlist")) return std::nullopt;
    if (!node->operator[]("volume")) return std::nullopt;
    if (!node->operator[]("loop")) return std::nullopt;

    return AudioPlayer{
        .playlist = node->operator[]("playlist").as<std::vector<AudioID>>(),
        .volume = node->operator[]("volume").as<f32>(),
        .loop = node->operator[]("loop").as<bool>()
    };
}