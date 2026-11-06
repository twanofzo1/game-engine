#pragma once
#include "core/types.hpp"

struct Sprite{
    std::string name;
    Texture* texture;
    Vec2 size;
    u16 h_frames;
    u16 v_frames;
    u16 frame;
};