#pragma once
#include "../../../typedefs.hpp"
#include <vector>

enum class AreaType {
    Circle,
    Rectangle,
    Polygon
};

struct AreaRectConfig {
    Vector2I size;
};

struct AreaCircleConfig {
    u32 radius;
};

struct AreaPolygonConfig {
    std::vector<std::pair<u32, u32>> points;
};


struct Area{
    AreaType type;
    union {
        struct {
            u32 radius;
        } circle;

        struct {
            Vector2I size;
        } rectangle;

        struct {
            std::vector<std::pair<u32, u32>> points;
        } polygon;
    };
};