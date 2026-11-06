#pragma once
#include "core/types.hpp"

struct Hitbox{
    std::string name;
    union{
        struct{
            Rect rect;
        };
        struct{
            Circle circle;
        };
    };
};
