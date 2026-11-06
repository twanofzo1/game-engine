#pragma once

#include "../serialisation.hpp"
#include "../deserialiser.hpp"




Yaml::Node Serialiser::serialise_color(const Color* color){
    Yaml::Node node;
    node["r"] = color->r;
    node["g"] = color->g;
    node["b"] = color->b;
    node["a"] = color->a;
    return node;
}

std::optional<Color> Deserialiser::deserialise_color(const Yaml::Node* node){
    if(!node->IsMap()) return std::nullopt;
    if(!node->operator[]("r")) return std::nullopt;
    if(!node->operator[]("g")) return std::nullopt;
    if(!node->operator[]("b")) return std::nullopt;
    if(!node->operator[]("a")) return std::nullopt;

    Color color;
    color.r = node->operator[]("r").as<float>();
    color.g = node->operator[]("g").as<float>();
    color.b = node->operator[]("b").as<float>();
    color.a = node->operator[]("a").as<float>();

    return color;
}   

