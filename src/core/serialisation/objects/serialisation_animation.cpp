/**
 * @file serialisation_animation.cpp
 * @brief Serialise/deserialise the Animation type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an animation into a YAML node. */
YamlNode Serialiser::serialise_animation(const Animation* animation)
{
    YamlNode node;
    node["id"] = animation->id;
    node["name"] = animation->name;

    YamlNode frames_node;
    for (const auto& frame : animation->frames)
    {
        YamlNode frame_node;
        frame_node["timestamp"] = frame.first;
        frame_node["frame"] = frame.second;
        frames_node.push_back(frame_node);
    }
    node["frames"] = frames_node;

    return node;
}

/** @brief Deserialise an animation from a YAML node, or none if invalid. */
std::optional<Animation> Deserialiser::deserialise_animation(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("id")) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("frames")) return std::nullopt;

    Animation animation = {};
    animation.id = node->operator[]("id").as<AnimationID>();
    animation.name = node->operator[]("name").as<std::string>();

    YamlNode frames_node = node->operator[]("frames");
    for (const YamlNode& frame_node : frames_node)
    {
        if (!frame_node.operator[]("timestamp")) return std::nullopt;
        if (!frame_node.operator[]("frame")) return std::nullopt;

        f32 timestamp = frame_node.operator[]("timestamp").as<f32>();
        u16 frame = frame_node.operator[]("frame").as<u16>();
        animation.frames.emplace_back(timestamp, frame);
    }

    return animation;
}
