#pragma once
#include "yaml-cpp/yaml.h"
#include <optional>

#include "core/objects.hpp"

#include "core/objects/area.hpp"
#include "core/objects/attack.hpp"
#include "core/objects/audio_file.hpp"
#include "core/objects/audio_player.hpp"
#include "core/objects/collision.hpp"
#include "core/objects/hitbox.hpp"
#include "core/objects/hurtbox.hpp"
#include "core/objects/animation_player.hpp"
#include "core/objects/sprite.hpp"
#include "core/objects/texture.hpp"

struct Deserialiser{
    static std::optional<Circle>          deserialise_circle(const Yaml::Node* node);
    static std::optional<Color>           deserialise_color(const Yaml::Node* node);
    static std::optional<Rect>            deserialise_rect(const Yaml::Node* node);
    static std::optional<Vec2>            deserialise_vec2(const Yaml::Node* node);
    static std::optional<Vec2i>           deserialise_vec2i(const Yaml::Node* node);
    static std::optional<Area>            deserialise_area(const Yaml::Node* node);
    static std::optional<Attack>          deserialise_attack(const Yaml::Node* node);
    static std::optional<AudioFile>       deserialise_audio_file(const Yaml::Node* node);
    static std::optional<AudioPlayer>     deserialise_audio_player(const Yaml::Node* node);
    static std::optional<Collision>       deserialise_collision(const Yaml::Node* node);
    static std::optional<Hitbox>          deserialise_hitbox(const Yaml::Node* node);
    static std::optional<Hurtbox>         deserialise_hurtbox(const Yaml::Node* node);
    static std::optional<AnimationPlayer> deserialise_animation_player(const Yaml::Node* node);
    static std::optional<Sprite>          deserialise_sprite(const Yaml::Node* node);
    static std::optional<Texture>         deserialise_texture(const Yaml::Node* node);
}