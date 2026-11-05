/**
 * @file attack.cpp
 * @brief Attack data and playback implementation.
 */
#include "gameplay/attack/attack.hpp"

#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Attack {

void init(Attack* attack)
{
    attack->name[0]       = '\0';
    attack->animation_id  = 0;
    attack->windup        = 0;
    attack->active        = 0;
    attack->recovery      = 0;

    attack->hitboxes       = NULL;
    attack->hitbox_count   = 0;
    attack->hitbox_capacity = 0;

    attack->events         = NULL;
    attack->event_count    = 0;
    attack->event_capacity = 0;

    attack->movements         = NULL;
    attack->movement_count    = 0;
    attack->movement_capacity = 0;
}

void destroy(Attack* attack)
{
    free(attack->hitboxes);
    free(attack->events);
    free(attack->movements);

    attack->hitboxes   = NULL;
    attack->events     = NULL;
    attack->movements  = NULL;
    attack->hitbox_count = attack->event_count = attack->movement_count = 0;
    attack->hitbox_capacity = attack->event_capacity = attack->movement_capacity = 0;
}

void add_hitbox(Attack* attack, i32 frame, i32 shape_id)
{
    if (attack->hitbox_count == attack->hitbox_capacity)
    {
        u32 next = attack->hitbox_capacity == 0 ? 8 : attack->hitbox_capacity * 2;
        attack->hitboxes = (HitboxKeyframe*)realloc(attack->hitboxes, sizeof(HitboxKeyframe) * next);
        attack->hitbox_capacity = next;
    }

    u32 i = attack->hitbox_count;
    while (i > 0 && attack->hitboxes[i - 1].frame > frame)
    {
        attack->hitboxes[i] = attack->hitboxes[i - 1];
        --i;
    }
    attack->hitboxes[i].frame     = frame;
    attack->hitboxes[i].shape_id = shape_id;
    ++attack->hitbox_count;
}

void add_event(Attack* attack, i32 frame, const char* name)
{
    if (attack->event_count == attack->event_capacity)
    {
        u32 next = attack->event_capacity == 0 ? 8 : attack->event_capacity * 2;
        attack->events = (EventKeyframe*)realloc(attack->events, sizeof(EventKeyframe) * next);
        attack->event_capacity = next;
    }

    u32 i = attack->event_count;
    while (i > 0 && attack->events[i - 1].frame > frame)
    {
        attack->events[i] = attack->events[i - 1];
        --i;
    }
    attack->events[i].frame = frame;
    strncpy(attack->events[i].name, name ? name : "", sizeof(attack->events[i].name) - 1);
    attack->events[i].name[sizeof(attack->events[i].name) - 1] = '\0';
    ++attack->event_count;
}

void add_movement(Attack* attack, i32 frame, Vec2::Vec2 vector)
{
    if (attack->movement_count == attack->movement_capacity)
    {
        u32 next = attack->movement_capacity == 0 ? 8 : attack->movement_capacity * 2;
        attack->movements = (MovementKeyframe*)realloc(attack->movements, sizeof(MovementKeyframe) * next);
        attack->movement_capacity = next;
    }

    u32 i = attack->movement_count;
    while (i > 0 && attack->movements[i - 1].frame > frame)
    {
        attack->movements[i] = attack->movements[i - 1];
        --i;
    }
    attack->movements[i].frame   = frame;
    attack->movements[i].vector = vector;
    ++attack->movement_count;
}

i32 duration(const Attack* attack)
{
    return attack->windup + attack->active + attack->recovery;
}

i32 hitbox_at(const Attack* attack, i32 frame)
{
    i32 shape = -1;
    for (u32 i = 0; i < attack->hitbox_count; ++i)
    {
        if (frame >= attack->hitboxes[i].frame)
            shape = attack->hitboxes[i].shape_id;
        else
            break;
    }
    return shape;
}

Vec2::Vec2 movement_at(const Attack* attack, i32 frame)
{
    Vec2::Vec2 vector = Vec2::make(0.0f, 0.0f);
    for (u32 i = 0; i < attack->movement_count; ++i)
    {
        if (frame >= attack->movements[i].frame)
            vector = attack->movements[i].vector;
        else
            break;
    }
    return vector;
}

void playback_start(Playback* playback, const Attack* attack)
{
    playback->attack     = attack;
    playback->frame      = 0;
    playback->next_event = 0;
}

void playback_update(Playback* playback, i32 frame, Event::Bus* events)
{
    if (!playback->attack)
        return;

    playback->frame = frame;

    while (playback->next_event < playback->attack->event_count &&
           playback->attack->events[playback->next_event].frame <= playback->frame)
    {
        Event::emit(events, playback->attack->events[playback->next_event].name, NULL);
        ++playback->next_event;
    }
}

} // namespace Attack
} // namespace Engine
