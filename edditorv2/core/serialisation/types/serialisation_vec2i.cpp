#pragma once

#include "../serialisation.hpp"
#include "../deserialiser.hpp"



Yaml::Node Serialiser::serialise_vec2i(const Vec2i* vec){
    Yaml::Node node;
    node["x"] = vec->x;
    node["y"] = vec->y;
    return node;
}

std::optional<Vec2i> Deserialiser::deserialise_vec2i(const Yaml::Node* node){
    if(!node->IsMap()) return std::nullopt;
    if(!node->operator[]("x")) return std::nullopt;
    if(!node->operator[]("y")) return std::nullopt;

    Vec2i vec;
    vec.x = node->operator[]("x").as<i32>();
    vec.y = node->operator[]("y").as<i32>();

    return vec;
}   

