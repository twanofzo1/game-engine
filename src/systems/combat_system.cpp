/**
 * @file combat_system.cpp
 * @brief Combat/trigger system: hitbox-hurtbox and area-area overlap resolution.
 *
 * Iterates the ECS component pools directly. A hitbox entity with one or more
 * shapes is tested against every hurtbox entity (whose shape is 1-to-1); each
 * overlap notifies both the hitbox's and the hurtbox's listeners. Areas are
 * tested pairwise against each other and notify both areas' listeners.
 */
#include "systems/combat_system.hpp"

#include "core/types/rect.hpp"

#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Systems {

void shape_to_world(const Collision::CollisionShape2D* local,
                    const Ecs::Transform* transform,
                    Collision::CollisionShape2D* out)
{
    *out = *local;

    if (local->type == Collision::SHAPE_CIRCLE)
    {
        out->pos = Transform2D::apply(*transform, local->pos);
    }
    else if (local->type == Collision::SHAPE_RECT)
    {
        Vec2::Vec2 center = Rect::center(local->rect);
        Vec2::Vec2 world  = Transform2D::apply(*transform, center);
        out->rect.position = Vec2::make(world.x - local->rect.size.x * 0.5f,
                                        world.y - local->rect.size.y * 0.5f);
        out->rotation = local->rotation + transform->rotation;
    }
    else
    {
        for (u32 i = 0; i < local->point_count; ++i)
            out->points[i] = Transform2D::apply(*transform, local->points[i]);
    }
}

/** @brief Grow a scratch buffer to hold `count` items of `item_size` bytes. */
static void scratch_grow(void** buf, u32* cap, u32 count, usize item_size)
{
    if (*cap >= count)
        return;
    free(*buf);
    *cap = count > 0 ? count : 1;
    *buf = malloc(item_size * (*cap));
}

void combat_system_update(Ecs::World* world)
{
    u32 hitbox_count  = Ecs::world_count(world, Ecs::COMPONENT_HITBOX);
    u32 hurtbox_count = Ecs::world_count(world, Ecs::COMPONENT_HURTBOX);
    u32 area_count    = Ecs::world_count(world, Ecs::COMPONENT_AREA);

    /* Scratch: per-entity "overlaps something this frame" + first payload. */
    static bool*    hb_any = NULL; static u32 hb_any_cap = 0;
    static Hit*     hb_hit = NULL; static u32 hb_hit_cap = 0;
    static bool*    ht_any = NULL; static u32 ht_any_cap = 0;
    static Hit*     ht_hit = NULL; static u32 ht_hit_cap = 0;
    static bool*    ar_any = NULL; static u32 ar_any_cap = 0;
    static Overlap* ar_ov  = NULL; static u32 ar_ov_cap  = 0;

    scratch_grow((void**)&hb_any, &hb_any_cap, hitbox_count, sizeof(bool));
    scratch_grow((void**)&hb_hit, &hb_hit_cap, hitbox_count, sizeof(Hit));
    scratch_grow((void**)&ht_any, &ht_any_cap, hurtbox_count, sizeof(bool));
    scratch_grow((void**)&ht_hit, &ht_hit_cap, hurtbox_count, sizeof(Hit));
    scratch_grow((void**)&ar_any, &ar_any_cap, area_count, sizeof(bool));
    scratch_grow((void**)&ar_ov,  &ar_ov_cap,  area_count, sizeof(Overlap));

    if (hitbox_count > 0)  memset(hb_any, 0, sizeof(bool) * hitbox_count);
    if (hurtbox_count > 0) memset(ht_any, 0, sizeof(bool) * hurtbox_count);
    if (area_count > 0)    memset(ar_any, 0, sizeof(bool) * area_count);

    /* 1. Hitbox vs hurtbox. */
    for (u32 i = 0; i < hitbox_count; ++i)
    {
        Ecs::Entity hb_entity = Ecs::world_entity_at(world, Ecs::COMPONENT_HITBOX, i);
        Ecs::Hitbox* hb = (Ecs::Hitbox*)Ecs::world_get_component(world, hb_entity, Ecs::COMPONENT_HITBOX);
        Ecs::Transform hb_t;
        if (!hb || !Ecs::world_get_transform(world, hb_entity, &hb_t))
            continue;

        for (u32 j = 0; j < hurtbox_count; ++j)
        {
            Ecs::Entity ht_entity = Ecs::world_entity_at(world, Ecs::COMPONENT_HURTBOX, j);
            Ecs::Hurtbox* ht = (Ecs::Hurtbox*)Ecs::world_get_component(world, ht_entity, Ecs::COMPONENT_HURTBOX);
            Ecs::Transform ht_t;
            if (!ht || !Ecs::world_get_transform(world, ht_entity, &ht_t))
                continue;

            bool overlap = false;
            Collision::CollisionShape2D wa, wb;
            for (u32 s = 0; s < hb->shape_count && !overlap; ++s)
            {
                shape_to_world(&hb->shapes[s], &hb_t, &wa);
                shape_to_world(&ht->shape, &ht_t, &wb);
                overlap = Collision::collide(&wa, &wb);
            }
            if (!overlap)
                continue;

            Hit hit;
            hit.attacker = hb_entity;
            hit.defender = ht_entity;

            if (hb->mode == Ecs::TRIGGER_CONSTANT)
                for (u32 s = 0; s < hb->listener_count; ++s)
                    hb->listeners[s](&hit);
            if (ht->mode == Ecs::TRIGGER_CONSTANT)
                for (u32 s = 0; s < ht->listener_count; ++s)
                    ht->listeners[s](&hit);

            if (!hb_any[i]) { hb_any[i] = true; hb_hit[i] = hit; }
            if (!ht_any[j]) { ht_any[j] = true; ht_hit[j] = hit; }
        }
    }

    /* 2. Fire once-per-overlap (ON_ENTER) listeners. */
    for (u32 i = 0; i < hitbox_count; ++i)
    {
        Ecs::Entity e = Ecs::world_entity_at(world, Ecs::COMPONENT_HITBOX, i);
        Ecs::Hitbox* hb = (Ecs::Hitbox*)Ecs::world_get_component(world, e, Ecs::COMPONENT_HITBOX);
        if (!hb || hb->mode != Ecs::TRIGGER_ON_ENTER)
            continue;
        if (hb_any[i] && !hb->active)
        {
            for (u32 s = 0; s < hb->listener_count; ++s)
                hb->listeners[s](&hb_hit[i]);
            hb->active = true;
        }
        else if (!hb_any[i])
        {
            hb->active = false;
        }
    }
    for (u32 j = 0; j < hurtbox_count; ++j)
    {
        Ecs::Entity e = Ecs::world_entity_at(world, Ecs::COMPONENT_HURTBOX, j);
        Ecs::Hurtbox* ht = (Ecs::Hurtbox*)Ecs::world_get_component(world, e, Ecs::COMPONENT_HURTBOX);
        if (!ht || ht->mode != Ecs::TRIGGER_ON_ENTER)
            continue;
        if (ht_any[j] && !ht->active)
        {
            for (u32 s = 0; s < ht->listener_count; ++s)
                ht->listeners[s](&ht_hit[j]);
            ht->active = true;
        }
        else if (!ht_any[j])
        {
            ht->active = false;
        }
    }

    /* 3. Area vs area. */
    for (u32 i = 0; i < area_count; ++i)
    {
        Ecs::Entity a_entity = Ecs::world_entity_at(world, Ecs::COMPONENT_AREA, i);
        Ecs::Area* ac = (Ecs::Area*)Ecs::world_get_component(world, a_entity, Ecs::COMPONENT_AREA);
        Ecs::Transform at;
        if (!ac || !Ecs::world_get_transform(world, a_entity, &at))
            continue;

        for (u32 j = i + 1; j < area_count; ++j)
        {
            Ecs::Entity b_entity = Ecs::world_entity_at(world, Ecs::COMPONENT_AREA, j);
            Ecs::Area* bc = (Ecs::Area*)Ecs::world_get_component(world, b_entity, Ecs::COMPONENT_AREA);
            Ecs::Transform bt;
            if (!bc || !Ecs::world_get_transform(world, b_entity, &bt))
                continue;

            Collision::CollisionShape2D wa, wb;
            shape_to_world(&ac->shape, &at, &wa);
            shape_to_world(&bc->shape, &bt, &wb);
            if (!Collision::collide(&wa, &wb))
                continue;

            Overlap overlap;
            overlap.owner_a = a_entity;
            overlap.owner_b = b_entity;

            if (ac->mode == Ecs::TRIGGER_CONSTANT)
                for (u32 s = 0; s < ac->listener_count; ++s)
                    ac->listeners[s](&overlap);
            if (bc->mode == Ecs::TRIGGER_CONSTANT)
                for (u32 s = 0; s < bc->listener_count; ++s)
                    bc->listeners[s](&overlap);

            if (!ar_any[i]) { ar_any[i] = true; ar_ov[i] = overlap; }
            if (!ar_any[j]) { ar_any[j] = true; ar_ov[j] = overlap; }
        }
    }

    for (u32 i = 0; i < area_count; ++i)
    {
        Ecs::Entity e = Ecs::world_entity_at(world, Ecs::COMPONENT_AREA, i);
        Ecs::Area* ar = (Ecs::Area*)Ecs::world_get_component(world, e, Ecs::COMPONENT_AREA);
        if (!ar || ar->mode != Ecs::TRIGGER_ON_ENTER)
            continue;
        if (ar_any[i] && !ar->active)
        {
            for (u32 s = 0; s < ar->listener_count; ++s)
                ar->listeners[s](&ar_ov[i]);
            ar->active = true;
        }
        else if (!ar_any[i])
        {
            ar->active = false;
        }
    }
}

} // namespace Systems
} // namespace Engine
