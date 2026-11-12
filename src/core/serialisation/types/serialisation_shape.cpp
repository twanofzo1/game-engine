/**
 * @file serialisation_shape.cpp
 * @brief Serialise/deserialise the Shape type to and from YAML.
 */
#include "../serialiser.hpp"
#include "../deserialiser.hpp"


/** @brief Serialise a shape into a YAML node. */
YamlNode Serialiser::serialise_shape(const Shape* shape)
{
    YamlNode node;
    node["type"] = static_cast<u32>(shape->type);
    if (shape->type == Shape::Type::Rect)
    {
        node["rect"] = Serialiser::serialise_rect(&shape->rect);
    }
    else
    {
        node["circle"] = Serialiser::serialise_circle(&shape->circle);
    }
    return node;
}

/** @brief Deserialise a shape from a YAML node, or none if invalid. */
std::optional<Shape> Deserialiser::deserialise_shape(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("type")) return std::nullopt;

    Shape::Type type = static_cast<Shape::Type>(node->operator[]("type").as<u32>());

    if (type == Shape::Type::Rect)
    {
        if (!node->operator[]("rect")) return std::nullopt;
        YamlNode rect_node = node->operator[]("rect");
        std::optional<Rect> rect = Deserialiser::deserialise_rect(&rect_node);
        if (!rect.has_value()) return std::nullopt;
        return Shape::make_rect(rect.value());
    }
    else
    {
        if (!node->operator[]("circle")) return std::nullopt;
        YamlNode circle_node = node->operator[]("circle");
        std::optional<Circle> circle = Deserialiser::deserialise_circle(&circle_node);
        if (!circle.has_value()) return std::nullopt;
        return Shape::make_circle(circle.value());
    }
}
