/**
 * @file animation.cpp
 * @brief Timeline animation implementation.
 */
#include "gameplay/animation/animation.hpp"

#include <math.h>
#include <stdlib.h>

namespace Engine {
namespace Animation {

void init(Animation* animation)
{
    animation->name[0]       = '\0';
    animation->sprite_id     = 0;
    animation->frames        = NULL;
    animation->frame_count   = 0;
    animation->frame_capacity = 0;
}

void destroy(Animation* animation)
{
    free(animation->frames);
    animation->frames        = NULL;
    animation->frame_count   = 0;
    animation->frame_capacity = 0;
}

void add_frame(Animation* animation, f32 time, i32 frame)
{
    if (animation->frame_count == animation->frame_capacity)
    {
        u32 next = animation->frame_capacity == 0 ? 8 : animation->frame_capacity * 2;
        animation->frames = (Keyframe*)realloc(animation->frames, sizeof(Keyframe) * next);
        animation->frame_capacity = next;
    }

    /* Insertion-sort by time; keyframe counts are tiny. */
    u32 i = animation->frame_count;
    while (i > 0 && animation->frames[i - 1].time > time)
    {
        animation->frames[i] = animation->frames[i - 1];
        --i;
    }
    animation->frames[i].time  = time;
    animation->frames[i].frame = frame;
    ++animation->frame_count;
}

f32 duration(const Animation* animation)
{
    if (animation->frame_count == 0)
        return 0.0f;
    return animation->frames[animation->frame_count - 1].time;
}

i32 frame_at(const Animation* animation, f32 time)
{
    if (animation->frame_count == 0)
        return 0;

    f32 d = duration(animation);
    if (d > 0.0f && time >= d)
        time = fmodf(time, d);

    i32 frame = animation->frames[0].frame;
    for (u32 i = 0; i < animation->frame_count; ++i)
    {
        if (time >= animation->frames[i].time)
            frame = animation->frames[i].frame;
        else
            break;
    }
    return frame;
}

} // namespace Animation
} // namespace Engine
