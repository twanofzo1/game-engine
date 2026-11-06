#pragma once
#include "sprite.hpp"
#include "core/typedefs.hpp"

struct Animation{
    std::string name;
    Sprite* sprite;
    std::vector<std::pair<f32, u16>> frames;
}

