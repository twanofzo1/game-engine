#pragma once
#include "core/typedefs.hpp"
#include "animation.hpp"

struct AnimationPlayer{
    std::string name;
    std::vector<Animation> animations;
    f32 speed;
};