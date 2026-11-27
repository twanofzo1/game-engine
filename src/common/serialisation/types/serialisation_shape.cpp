#include "../serialiser.hpp"
#include "../deserialiser.hpp"

YamlNode Serialiser::serialise_shape(const Shape* shape)
{
    YamlNode node;
    node["type"] = static_cast<u32>(shape->type);
    if (shape->type == ShapeType::Rect)
    {
        node["rect"] = Serialiser::serialise_rect(&shape->rect);
    }
    else
    {
        node["circle"] = Serialiser::serialise_circle(&shape->circle);
    }
    return node;
}

/** @brief Deserialise a collision shape from a YAML node. */
std::optional<Shape> Deserialiser::deserialise_shape(const YamlNode* node)
{
    if (!node->IsMap()) return std::nullopt;
    if (!node->operator[]("type")) return std::nullopt;
    if (!node->operator[]("shape")) return std::nullopt;

    Shape shape = {};
    shape.type = static_cast<ShapeType>(node->operator[]("shape").as<u32>());

    if (shape.type == ShapeType::Rect)
    {
        YamlNode rect_node = node->operator[]("rect");
        std::optional<Rect> rect = Deserialiser::deserialise_rect(&rect_node);
        if (!rect.has_value()) return std::nullopt;
        shape.rect = rect.value();
    }
    else
    {
        YamlNode circle_node = node->operator[]("circle");
        std::optional<Circle> circle = Deserialiser::deserialise_circle(&circle_node);
        if (!circle.has_value()) return std::nullopt;
        shape.circle = circle.value();
    }

    return shape;
}