#pragma once
#include "core/types/types.hpp"

struct Collision{
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