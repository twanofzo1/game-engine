#pragma once
#include "yaml-cpp/yaml.h"

#include "core/types/circle.hpp"
#include "core/types/color.hpp"
#include "core/types/rect.hpp"
#include "core/types/vec2.hpp"
#include "core/types/vec2i.hpp"

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

struct Serialiser{
    static Yaml::Node serialise_circle(const Circle* circle);
    static Yaml::Node serialise_color(const Color* color);
    static Yaml::Node serialise_rect(const Rect* rect);
    static Yaml::Node serialise_vec2(const Vec2* vec);
    static Yaml::Node serialise_vec2i(const Vec2i* vec);
    static Yaml::Node serialise_area(const Area* area);
    static Yaml::Node serialise_attack(const Attack* attack);  
    static Yaml::Node serialise_audioFile(const AudioFile* audioFile);
    static Yaml::Node serialise_audioPlayer(const AudioPlayer* audioPlayer);
    static Yaml::Node serialise_collision(const Collision* collision);
    static Yaml::Node serialise_hitbox(const Hitbox* hitbox);
    static Yaml::Node serialise_hurtbox(const Hurtbox* hurtbox);
    static Yaml::Node serialise_animationPlayer(const AnimationPlayer* animationPlayer);
    static Yaml::Node serialise_sprite(const Sprite* sprite);
    static Yaml::Node serialise_texture(const Texture* texture);
}

