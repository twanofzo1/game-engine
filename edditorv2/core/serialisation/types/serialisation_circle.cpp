#pragma once

#include "../serialisation.hpp"
#include "../deserialiser.hpp"



Yaml::Node Serialiser::serialise_circle(const Circle* circle){
    Yaml::Node node;
    node["center"] = Serialiser::serialise_vec2(&circle->center);
    node["radius"] = circle->radius;
    return node;
}

std::optional<Circle> Deserialiser::deserialise_circle(const Yaml::Node* node){
    if(!node->IsMap()) return std::nullopt;
    if(!node->operator[]("center")) return std::nullopt;
    if(!node->operator[]("radius")) return std::nullopt;

    std::optional<Vec2> center = Deserialiser::deserialise_vec2(&node->operator[]("center"));
    if(!center.has_value()) return std::nullopt;

    std::optional<float> radius = node->operator[]("radius").as<float>();
    if(!radius.has_value()) return std::nullopt;

    Circle circle;
    circle.center = center.value();
    circle.radius = radius.value();

    return circle;
}   

