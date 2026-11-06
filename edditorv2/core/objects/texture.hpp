#pragma once
#include "core/types.hpp"

struct Texture{
    std::string name;
    std::string path;
    Vec2i size;
    u8* data;
};