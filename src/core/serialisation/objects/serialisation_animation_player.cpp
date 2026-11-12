/**
 * @file serialisation_animation_player.cpp
 * @brief Serialise/deserialise the AnimationPlayer type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an animation player into a YAML node. */
YamlNode Serialiser::serialise_animation_player(const AnimationPlayer* animation_player)
{
    YamlNode node;
    node["name"] = animation_player->name;
    node["sprite"] = animation_player->sprite;
    node["speed"] = animation_player->speed;

    YamlNode animations_node;
    for (const Animation& animation : animation_player->animations)
    {
        animations_node.push_back(Serialiser::serialise_animation(&animation));
    }
    node["animations"] = animations_node;

    return node;
}

/** @brief Deserialise an animation player from a YAML node. */
std::optional<AnimationPlayer> Deserialiser::deserialise_animation_player(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("sprite")) return std::nullopt;
    if (!node->operator[]("speed")) return std::nullopt;
    if (!node->operator[]("animations")) return std::nullopt;

    AnimationPlayer player = {};
    player.name = node->operator[]("name").as<std::string>();
    player.sprite = node->operator[]("sprite").as<SpriteID>();
    player.speed = node->operator[]("speed").as<f32>();

    YamlNode animations_node = node->operator[]("animations");
    for (const YamlNode& animation_node : animations_node)
    {
        std::optional<Animation> animation = Deserialiser::deserialise_animation(&animation_node);
        if (!animation.has_value()) return std::nullopt;
        player.animations.push_back(animation.value());
    }

    return player;
}