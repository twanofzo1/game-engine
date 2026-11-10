/**
 * @file serialisation_audio_file.cpp
 * @brief Serialise/deserialise the AudioFile type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an audio file into a YAML node. */
YamlNode Serialiser::serialise_audio_file(const AudioFile* audio_file)
{
    (void)audio_file;
    return YamlNode();
}

/** @brief Deserialise an audio file from a YAML node. */
std::optional<AudioFile> Deserialiser::deserialise_audio_file(const YamlNode* node)
{
    (void)node;
    return std::nullopt;
}