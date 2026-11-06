#pragma once

#include "../serialisation.hpp"
#include "../deserialiser.hpp"




Yaml::Node Serialiser::serialise_rect(const Rect* rect){
    Yaml::Node node;
    node["position"] = Serialiser::serialise_vec2(&rect->position);
    node["size"] = Serialiser::serialise_vec2(&rect->size);
    return node;
}

std::optional<Rect> Deserialiser::deserialise_rect(const Yaml::Node* node){
    if(!node->IsMap()) return std::nullopt;
    if(!node->operator[]("position")) return std::nullopt;
    if(!node->operator[]("size")) return std::nullopt;

    std::optional<Vec2> position = Deserialiser::deserialise_vec2(&node->operator[]("position"));
    if(!position.has_value()) return std::nullopt;

    std::optional<Vec2> size = Deserialiser::deserialise_vec2(&node->operator[]("size"));
    if(!size.has_value()) return std::nullopt;

    Rect rect;
    rect.position = position.value();
    rect.size = size.value();

    return rect;
}   

