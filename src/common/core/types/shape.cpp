/**
 * @file shape.cpp
 * @brief Implementation of the Shape geometric figure type.
 */
#include "shape.hpp"

/// @brief Constructs a rectangular shape.
/// @param rect The rectangle to store.
/// @return The rectangular shape.
Shape Shape::create(const Rect rect)
{
    Shape shape = {};
    shape.type = Shape::Type::Rect;
    shape.rect = rect;
    return shape;
}

/// @brief Constructs a circular shape.
/// @param circle The circle to store.
/// @return The circular shape.
Shape Shape::create(const Circle circle)
{
    Shape shape = {};
    shape.type = Shape::Type::Circle;
    shape.circle = circle;
    return shape;
}
