/**
 * @file serialisation_audio_file.cpp
 * @brief Serialise/deserialise the AudioFile type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an audio file into a YAML node. */
YamlNode Serialiser::serialise_audio_file(const AudioFile* audio_file)
{
    YamlNode node;
    node["id"] = audio_file->id;
    node["name"] = audio_file->name;
    node["path"] = audio_file->path;
    return node;
}

/** @brief Deserialise an audio file from a YAML node. */
std::optional<AudioFile> Deserialiser::deserialise_audio_file(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("id")) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("path")) return std::nullopt;

    return AudioFile{
        .id = node->operator[]("id").as<AudioID>(),
        .name = node->operator[]("name").as<std::string>(),
        .path = node->operator[]("path").as<std::string>()
    };
}