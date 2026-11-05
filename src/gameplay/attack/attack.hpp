/**
 * @file attack.hpp
 * @brief Attack data: phased timing plus hitbox/core/event/movement keyframes.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "core/event/event.hpp"

namespace Engine {
namespace Attack {

/** A collision-shape switch: at `frame` the active shape becomes `shape_id`. */
struct HitboxKeyframe {
    i32 frame;
    i32 shape_id;
};

/** An event fired at `frame` with a name. */
struct EventKeyframe {
    i32  frame;
    char name[32];
};

/** A movement vector applied at `frame`. */
struct MovementKeyframe {
    i32        frame;
    Vec2::Vec2 vector;
};

/** An attack definition with frame-based keyframe timelines. */
struct Attack {
    char name[32];
    i32  animation_id;
    i32  windup;   ///< Windup phase length in frames.
    i32  active;   ///< Active phase length in frames.
    i32  recovery; ///< Recovery phase length in frames.

    HitboxKeyframe*   hitboxes;
    u32               hitbox_count;
    u32               hitbox_capacity;

    EventKeyframe*    events;
    u32               event_count;
    u32               event_capacity;

    MovementKeyframe* movements;
    u32               movement_count;
    u32               movement_capacity;
};

void init(Attack* attack);
void destroy(Attack* attack);

void add_hitbox(Attack* attack, i32 frame, i32 shape_id);
void add_event(Attack* attack, i32 frame, const char* name);
void add_movement(Attack* attack, i32 frame, Vec2::Vec2 vector);

/** @brief Total length in frames = windup + active + recovery. */
i32 duration(const Attack* attack);

/** @brief Active hitbox shape at `frame`, or -1 before the first keyframe. */
i32 hitbox_at(const Attack* attack, i32 frame);

/** @brief Movement vector at `frame`. */
Vec2::Vec2 movement_at(const Attack* attack, i32 frame);

/** Playback state that fires each event exactly once as frames advance. */
struct Playback {
    const Attack* attack;
    i32  frame;
    u32  next_event;
};

void playback_start(Playback* playback, const Attack* attack);
void playback_update(Playback* playback, i32 frame, Event::Bus* events);

} // namespace Attack
} // namespace Engine
