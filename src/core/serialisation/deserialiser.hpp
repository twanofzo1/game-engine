/**
 * @file deserialiser.hpp
 * @brief Deserialiser: converts YAML nodes back into engine objects.
 */
#pragma once
#include <optional>

#include "core/serialisation/yaml.hpp"
#include "core/types.hpp"
#include "core/objects.hpp"

/** @brief Rebuilds engine objects from YAML nodes. */
struct Deserialiser {
    static std::optional<Circle>          deserialise_circle(const YamlNode* node);
    static std::optional<Color>           deserialise_color(const YamlNode* node);
    static std::optional<Rect>            deserialise_rect(const YamlNode* node);
    static std::optional<Vec2>            deserialise_vec2(const YamlNode* node);
    static std::optional<Vec2i>           deserialise_vec2i(const YamlNode* node);
    static std::optional<Area>            deserialise_area(const YamlNode* node);
    static std::optional<Attack>          deserialise_attack(const YamlNode* node);
    static std::optional<AudioFile>       deserialise_audio_file(const YamlNode* node);
    static std::optional<AudioPlayer>     deserialise_audio_player(const YamlNode* node);
    static std::optional<Collision>       deserialise_collision(const YamlNode* node);
    static std::optional<Hitbox>          deserialise_hitbox(const YamlNode* node);
    static std::optional<Hurtbox>         deserialise_hurtbox(const YamlNode* node);
    static std::optional<AnimationPlayer> deserialise_animation_player(const YamlNode* node);
    static std::optional<Sprite>          deserialise_sprite(const YamlNode* node);
    static std::optional<Texture>         deserialise_texture(const YamlNode* node);
};