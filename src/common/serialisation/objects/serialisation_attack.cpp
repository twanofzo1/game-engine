/**
 * @file serialisation_attack.cpp
 * @brief Serialise/deserialise the Attack type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise an attack into a YAML node. */
YamlNode Serialiser::serialise_attack(const Attack* attack)
{
    YamlNode node;
    node["animation"] = attack->animation;
    node["damage"] = attack->damage;
    node["knockback_multiplier"] = attack->knockback_multiplier;

    YamlNode hitboxes_node;
    for (const Hitbox& hitbox : attack->hitboxes)
    {
        hitboxes_node.push_back(Serialiser::serialise_hitbox(&hitbox));
    }
    node["hitboxes"] = hitboxes_node;

    YamlNode directions_node;
    for (const Vec2& dir : attack->knockback_directions)
    {
        directions_node.push_back(Serialiser::serialise_vec2(&dir));
    }
    node["knockback_directions"] = directions_node;

    YamlNode signals_node;
    for (const Signal& signal : attack->signals)
    {
        signals_node.push_back(Serialiser::serialise_signal(&signal));
    }
    node["signals"] = signals_node;

    return node;
}

/** @brief Deserialise an attack from a YAML node. */
std::optional<Attack> Deserialiser::deserialise_attack(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("animation")) return std::nullopt;
    if (!node->operator[]("damage")) return std::nullopt;
    if (!node->operator[]("knockback_multiplier")) return std::nullopt;
    if (!node->operator[]("hitboxes")) return std::nullopt;
    if (!node->operator[]("knockback_directions")) return std::nullopt;
    if (!node->operator[]("signals")) return std::nullopt;

    Attack attack = {};
    attack.animation = node->operator[]("animation").as<AnimationID>();
    attack.damage = node->operator[]("damage").as<u32>();
    attack.knockback_multiplier = node->operator[]("knockback_multiplier").as<u32>();

    YamlNode hitboxes_node = node->operator[]("hitboxes");
    for (const YamlNode& hitbox_node : hitboxes_node)
    {
        std::optional<Hitbox> hitbox = Deserialiser::deserialise_hitbox(&hitbox_node);
        if (!hitbox.has_value()) return std::nullopt;
        attack.hitboxes.push_back(hitbox.value());
    }

    YamlNode directions_node = node->operator[]("knockback_directions");
    for (const YamlNode& dir_node : directions_node)
    {
        std::optional<Vec2> dir = Deserialiser::deserialise_vec2(&dir_node);
        if (!dir.has_value()) return std::nullopt;
        attack.knockback_directions.push_back(dir.value());
    }

    YamlNode signals_node = node->operator[]("signals");
    for (const YamlNode& signal_node : signals_node)
    {
        std::optional<Signal> signal = Deserialiser::deserialise_signal(&signal_node);
        if (!signal.has_value()) return std::nullopt;
        attack.signals.push_back(signal.value());
    }

    return attack;
}