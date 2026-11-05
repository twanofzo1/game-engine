/**
 * @file animation.hpp
 * @brief Timeline animation: named keyframes played back on a sprite.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Animation {

/** One keyframe: at `time` seconds the animation shows `frame`. */
struct Keyframe {
    f32 time;  ///< Time in seconds from the start of the animation.
    i32 frame; ///< Sprite frame to show.
};

/** A named animation that drives a sprite (referenced by index). */
struct Animation {
    char      name[32];    ///< Animation name.
    i32       sprite_id;   ///< Index into the sprite list this drives.
    Keyframe* frames;      ///< Owned keyframe array (sorted by time).
    u32       frame_count;
    u32       frame_capacity;
};

void init(Animation* animation);
void destroy(Animation* animation);

/** @brief Append a keyframe; frames stay sorted by time. */
void add_frame(Animation* animation, f32 time, i32 frame);

/** @brief Duration of the animation in seconds (time of the last keyframe). */
f32 duration(const Animation* animation);

/**
 * @brief Frame to show at `time`, wrapping around so the animation loops.
 */
i32 frame_at(const Animation* animation, f32 time);

} // namespace Animation
} // namespace Engine
