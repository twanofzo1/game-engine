/**
 * @file serialisation_health.cpp
 * @brief Serialise/deserialise the Health type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a health value into a YAML node. */
YamlNode Serialiser::serialise_health(const Health* health)
{
    YamlNode node;
    node["initial"] = health->initial;
    node["max"] = health->max;
    node["on_death"] = Serialiser::serialise_signal(&health->on_death);
    node["on_damage"] = Serialiser::serialise_signal(&health->on_damage);
    node["on_heal"] = Serialiser::serialise_signal(&health->on_heal);
    return node;
}

/** @brief Deserialise a health value from a YAML node, or none if invalid. */
std::optional<Health> Deserialiser::deserialise_health(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("initial")) return std::nullopt;
    if (!node->operator[]("max")) return std::nullopt;
    if (!node->operator[]("on_death")) return std::nullopt;
    if (!node->operator[]("on_damage")) return std::nullopt;
    if (!node->operator[]("on_heal")) return std::nullopt;

    YamlNode on_death_node = node->operator[]("on_death");
    std::optional<Signal> on_death = Deserialiser::deserialise_signal(&on_death_node);
    if (!on_death.has_value()) return std::nullopt;

    YamlNode on_damage_node = node->operator[]("on_damage");
    std::optional<Signal> on_damage = Deserialiser::deserialise_signal(&on_damage_node);
    if (!on_damage.has_value()) return std::nullopt;

    YamlNode on_heal_node = node->operator[]("on_heal");
    std::optional<Signal> on_heal = Deserialiser::deserialise_signal(&on_heal_node);
    if (!on_heal.has_value()) return std::nullopt;

    return Health{
        .initial = node->operator[]("initial").as<u16>(),
        .max = node->operator[]("max").as<u16>(),
        .on_death = on_death.value(),
        .on_damage = on_damage.value(),
        .on_heal = on_heal.value()
    };
}
