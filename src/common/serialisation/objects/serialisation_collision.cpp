/**
 * @file serialisation_collision.cpp
 * @brief Serialise/deserialise the Collision type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a collision shape into a YAML node. */
YamlNode Serialiser::serialise_collision(const Collision* collision)
{
    YamlNode node;
    node["name"] = collision->name;
    node["shape"] = static_cast<u32>(collision->shape);
    if (collision->shape == CollisionShape::Rect)
    {
        node["rect"] = Serialiser::serialise_rect(&collision->rect);
    }
    else
    {
        node["circle"] = Serialiser::serialise_circle(&collision->circle);
    }
    return node;
}

/** @brief Deserialise a collision shape from a YAML node. */
std::optional<Collision> Deserialiser::deserialise_collision(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("name")) return std::nullopt;
    if (!node->operator[]("shape")) return std::nullopt;

    Collision collision = {};
    collision.name = node->operator[]("name").as<std::string>();
    collision.shape = static_cast<CollisionShape>(node->operator[]("shape").as<u32>());

    if (collision.shape == CollisionShape::Rect)
    {
        if (!node->operator[]("rect")) return std::nullopt;
        YamlNode rect_node = node->operator[]("rect");
        std::optional<Rect> rect = Deserialiser::deserialise_rect(&rect_node);
        if (!rect.has_value()) return std::nullopt;
        collision.rect = rect.value();
    }
    else
    {
        if (!node->operator[]("circle")) return std::nullopt;
        YamlNode circle_node = node->operator[]("circle");
        std::optional<Circle> circle = Deserialiser::deserialise_circle(&circle_node);
        if (!circle.has_value()) return std::nullopt;
        collision.circle = circle.value();
    }

    return collision;
}