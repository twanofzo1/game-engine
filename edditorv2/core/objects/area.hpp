#pragma once
#include "core/typedefs.hpp"

struct Area{
    std::string name;
    union{
        struct{
            Rect rect;
        };
        struct{
            Circle circle;
        };
    };
    Listener* listeners;
    u16 listener_count;
};