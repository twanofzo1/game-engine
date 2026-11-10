/**
 * @file shape.cpp
 * @brief Implementation of the Shape geometric figure type.
 */
#include "shape.hpp"


/** @brief Construct a rectangular shape. */
Shape Shape::make_rect(const Rect rect)
{
    Shape shape = {};
    shape.type = Shape::Type::Rect;
    shape.rect = rect;
    return shape;
}

/** @brief Construct a circular shape. */
Shape Shape::make_circle(const Circle circle)
{
    Shape shape = {};
    shape.type = Shape::Type::Circle;
    shape.circle = circle;
    return shape;
}
