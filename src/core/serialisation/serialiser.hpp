/**
 * @file serialiser.hpp
 * @brief Serialiser: converts engine objects into YAML nodes.
 */
#pragma once
#include "core/serialisation/yaml.hpp"
#include "core/types.hpp"
#include "core/objects.hpp"

/** @brief Builds YAML nodes from engine objects. */
struct Serialiser {
    static YamlNode serialise_circle(const Circle* circle);
    static YamlNode serialise_color(const Color* color);
    static YamlNode serialise_rect(const Rect* rect);
    static YamlNode serialise_vec2(const Vec2* vec);
    static YamlNode serialise_vec2i(const Vec2i* vec);
    static YamlNode serialise_area(const Area* area);
    static YamlNode serialise_attack(const Attack* attack);
    static YamlNode serialise_audio_file(const AudioFile* audio_file);
    static YamlNode serialise_audio_player(const AudioPlayer* audio_player);
    static YamlNode serialise_collision(const Collision* collision);
    static YamlNode serialise_hitbox(const Hitbox* hitbox);
    static YamlNode serialise_hurtbox(const Hurtbox* hurtbox);
    static YamlNode serialise_animation_player(const AnimationPlayer* animation_player);
    static YamlNode serialise_sprite(const Sprite* sprite);
    static YamlNode serialise_texture(const Texture* texture);
};
