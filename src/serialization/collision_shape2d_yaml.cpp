/**
 * @file collision_shape2d_yaml.cpp
 * @brief YAML serialization for CollisionShape2D, built on the Yaml helpers.
 */
#include "serialization/collision_shape2d_yaml.hpp"

namespace Yaml {

bool load_collision_shape(const Node* node, Engine::Collision::CollisionShape2D* out)
{
    if (out == NULL)
        return false;
    if (node == NULL || !node->IsMap())
        return false;

    std::string type = as_string((*node)["type"], "circle");

    if (type == "rect")
    {
        Engine::Vec2::Vec2 position = as_vec2((*node)["position"], Engine::Vec2::make(0.0f, 0.0f));
        Engine::Vec2::Vec2 size     = as_vec2((*node)["size"], Engine::Vec2::make(0.0f, 0.0f));
        f32 rotation = as_float((*node)["rotation"], 0.0f);
        Engine::Collision::make_rect(out, Engine::Rect::make(position.x, position.y, size.x, size.y), rotation);
    }
    else if (type == "polygon")
    {
        Node points = (*node)["points"];
        u32 count = points.IsSequence() ? (u32)points.size() : 0u;
        if (count > COLLISION_POLYGON_MAX_POINTS)
            count = COLLISION_POLYGON_MAX_POINTS;

        Engine::Vec2::Vec2 tmp[COLLISION_POLYGON_MAX_POINTS];
        for (u32 i = 0; i < count; ++i)
            tmp[i] = as_vec2(points[i], Engine::Vec2::make(0.0f, 0.0f));
        Engine::Collision::make_polygon(out, tmp, count);
    }
    else
    {
        Engine::Vec2::Vec2 center = as_vec2((*node)["center"], Engine::Vec2::make(0.0f, 0.0f));
        f32 radius = as_float((*node)["radius"], 0.0f);
        Engine::Collision::make_circle(out, center, radius);
    }

    return true;
}

Node make_collision_shape(const Engine::Collision::CollisionShape2D* shape)
{
    Node node;

    if (shape->type == Engine::Collision::SHAPE_RECT)
    {
        node["type"]     = std::string("rect");
        node["position"] = make_vec2(shape->rect.position);
        node["size"]     = make_vec2(shape->rect.size);
        node["rotation"] = shape->rotation;
    }
    else if (shape->type == Engine::Collision::SHAPE_POLYGON)
    {
        node["type"] = std::string("polygon");
        Node points;
        for (u32 i = 0; i < shape->point_count; ++i)
            points.push_back(make_vec2(shape->points[i]));
        node["points"] = points;
    }
    else
    {
        node["type"]   = std::string("circle");
        node["center"] = make_vec2(shape->pos);
        node["radius"] = shape->radius;
    }

    return node;
}

} // namespace Yaml
