#pragma once
#include "core/types.hpp"

struct Hurtbox{
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