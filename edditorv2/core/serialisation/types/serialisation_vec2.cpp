#pragma once

#include "../serialisation.hpp"
#include "../deserialiser.hpp"




Yaml::Node Serialiser::serialise_vec2(const Vec2* vec){
    Yaml::Node node;
    node["x"] = vec->x;
    node["y"] = vec->y;
    return node;
}

std::optional<Vec2> Deserialiser::deserialise_vec2(const Yaml::Node* node){
    if(!node->IsMap()) return std::nullopt;
    if(!node->operator[]("x")) return std::nullopt;
    if(!node->operator[]("y")) return std::nullopt;

    Vec2 vec;
    vec.x = node->operator[]("x").as<f32>();
    vec.y = node->operator[]("y").as<f32>();

    return vec;
}   

