/**
 * @file serialisation_hurtbox.cpp
 * @brief Serialise/deserialise the Hurtbox type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a hurtbox into a YAML node. */
YamlNode Serialiser::serialise_hurtbox(const Hurtbox* hurtbox)
{
    YamlNode node;
    node["name"] = hurtbox->name;
    node["shape"] = static_cast<u32>(hurtbox->shape);
    if (hurtbox->shape == HurtboxShape::Rect)
    {
        node["rect"] = Serialiser::serialise_rect(&hurtbox->rect);
    }
    else
    {
        node["circle"] = Serialiser::serialise_circle(&hurtbox->circle);
    }
    return node;
}

/** @brief Deserialise a hurtbox from a YAML node. */
std::optional<Hurtbox> Deserialiser::deserialise_hurtbox(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("shape")) return std::nullopt;

    Hurtbox hurtbox = {};
    hurtbox.name = node->operator[]("name").as<std::string>();
    hurtbox.shape = static_cast<HurtboxShape>(node->operator[]("shape").as<u32>());

    if (hurtbox.shape == HurtboxShape::Rect)
    {
        if (!node->operator[]("rect")) return std::nullopt;
        YamlNode rect_node = node->operator[]("rect");
        std::optional<Rect> rect = Deserialiser::deserialise_rect(&rect_node);
        if (!rect.has_value()) return std::nullopt;
        hurtbox.rect = rect.value();
    }
    else
    {
        if (!node->operator[]("circle")) return std::nullopt;
        YamlNode circle_node = node->operator[]("circle");
        std::optional<Circle> circle = Deserialiser::deserialise_circle(&circle_node);
        if (!circle.has_value()) return std::nullopt;
        hurtbox.circle = circle.value();
    }

    return hurtbox;
}