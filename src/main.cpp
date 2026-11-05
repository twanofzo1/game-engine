/**
 * @file main.cpp
 * @brief Demo entry point: initializes the engine and runs the render loop.
 *
 * Creates a GLFW window with an OpenGL 3.3 core context, loads GL with GLAD,
 * initializes an ECS world and the 2D batch renderer, spawns a small animated
 * sprite scene and renders it each frame with a Dear ImGui stats overlay.
 *
 * Usage:
 *   app                              # interactive window
 *   app --frames N                   # render exactly N frames, then exit
 *   app --stress N [--frames F]      # spawn N sprites to stress the renderer
 *   app --debug-boxes                # draw hit/hurt/area boxes as overlays
 */

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "core/ecs/world.hpp"
#include "core/ecs/components.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"
#include "core/types/rect.hpp"
#include "gameplay/collision/collision_shape2d.hpp"
#include "graphics/rendering/renderer.hpp"
#include "graphics/rendering/camera.hpp"
#include "systems/render_system.hpp"
#include "platform/input/input.hpp"
#include "platform/window/window.hpp"
#include "graphics/rendering/texture.hpp"
#include "resources/image.hpp"
#include "audio/manager.hpp"
#include "core/timers/timers.hpp"
#include "serialization/yaml.hpp"
#include "serialization/window_yaml.hpp"
#include "serialization/texture_yaml.hpp"
#include "serialization/sprite_yaml.hpp"
#include "serialization/animation_yaml.hpp"
#include "serialization/attack_yaml.hpp"
#include "gameplay/animation/animation.hpp"
#include "gameplay/attack/attack.hpp"
#include "core/event/event.hpp"
#include "systems/combat_system.hpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

/** Maximum number of textures the config file may define. */
#define MAX_CONFIG_TEXTURES 16

/** Maximum number of sprites / animations the config file may define. */
#define MAX_CONFIG_SPRITES 64
#define MAX_CONFIG_ANIMATIONS 64

/** Maximum number of attacks the config file may define. */
#define MAX_CONFIG_ATTACKS 16

using namespace Engine; // only permitted namespace since its for usage not implementation

/** @brief Logs a combat hit (payload = `const Systems::Hit*`). */
static void log_hit(void *payload)
{
    Systems::Hit *hit = (Systems::Hit *)payload;
    fprintf(stderr, "[hit] attacker %u -> defender %u\n", hit->attacker.id, hit->defender.id);
}

static void log_hurt(void *payload)
{
    Systems::Hit *hit = (Systems::Hit *)payload;
    fprintf(stderr, "[hurt] defender %u -> attacker %u\n", hit->defender.id, hit->attacker.id);
}

/** @brief Logs an area overlap (payload = `const Systems::Overlap*`). */
static void log_overlap(void *payload)
{
    Systems::Overlap *overlap = (Systems::Overlap *)payload;
    fprintf(stderr, "[overlap] owner %u <-> owner %u\n",
            overlap->owner_a.id, overlap->owner_b.id);
}

/**
 * @brief GLFW error callback; prints the error to stderr.
 */
static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

/**
 * @brief Parse the optional `--frames N` and `--stress N` arguments.
 * @param argc       Number of program arguments.
 * @param argv       Program argument vector.
 * @param out_frames Receives N when `--frames N` is present, otherwise 0.
 * @param out_stress Receives N when `--stress N` is present, otherwise 0.
 */
static void parse_args(int argc, char **argv, int *out_frames, int *out_stress, bool *out_debug_boxes)
{
    *out_frames = 0;
    *out_stress = 0;
    *out_debug_boxes = false;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc)
            *out_frames = atoi(argv[i + 1]);
        else if (strcmp(argv[i], "--stress") == 0 && i + 1 < argc)
            *out_stress = atoi(argv[i + 1]);
        else if (strcmp(argv[i], "--debug-boxes") == 0)
            *out_debug_boxes = true;
    }
}

/** @brief Push one transparent solid-color debug quad. */
static void debug_rect(Render::Renderer *renderer, Vec2::Vec2 center, Vec2::Vec2 size,
                       f32 rotation, Color::Color color)
{
    Ecs::Transform transform = Transform2D::make(center, rotation, Vec2::make(1.0f, 1.0f));
    Ecs::Sprite   sprite    = Sprite::make(0, size, 1, 1);
    sprite.color = color;
    Render::renderer_push_quad(renderer, &transform, &sprite);
}

/** @brief Draw one collision shape as a transparent debug overlay. */
static void debug_shape(Render::Renderer *renderer, const Collision::CollisionShape2D *shape,
                        Color::Color color)
{
    if (shape->type == Collision::SHAPE_RECT)
    {
        debug_rect(renderer, Rect::center(shape->rect), shape->rect.size, shape->rotation, color);
    }
    else if (shape->type == Collision::SHAPE_POLYGON)
    {
        Vec2::Vec2 mn, mx;
        Collision::bounds(shape, &mn, &mx);
        debug_rect(renderer, Vec2::scale(Vec2::add(mn, mx), 0.5f), Vec2::sub(mx, mn), 0.0f, color);
    }
    else
    {
        debug_rect(renderer, shape->pos, Vec2::make(shape->radius * 2.0f, shape->radius * 2.0f),
                   0.0f, color);
    }
}

/**
 * @brief Draw every hitbox/hurtbox/area component as a transparent overlay.
 * @param renderer Renderer to queue quads into.
 * @param world    World to read components from.
 */
static void draw_debug_world(Render::Renderer *renderer, Ecs::World *world)
{
    Color::Color hit_color  = Color::make(1.0f, 0.25f, 0.25f, 0.35f);
    Color::Color hurt_color = Color::make(0.25f, 0.45f, 1.0f, 0.35f);
    Color::Color area_color = Color::make(0.25f, 1.0f, 0.35f, 0.35f);

    u32 hb_count = Ecs::world_count(world, Ecs::COMPONENT_HITBOX);
    for (u32 i = 0; i < hb_count; ++i)
    {
        Ecs::Entity e = Ecs::world_entity_at(world, Ecs::COMPONENT_HITBOX, i);
        Ecs::Hitbox *hb = (Ecs::Hitbox *)Ecs::world_get_component(world, e, Ecs::COMPONENT_HITBOX);
        Ecs::Transform wt;
        if (!hb || !Ecs::world_get_transform(world, e, &wt))
            continue;
        for (u32 s = 0; s < hb->shape_count; ++s)
        {
            Collision::CollisionShape2D w;
            Systems::shape_to_world(&hb->shapes[s], &wt, &w);
            debug_shape(renderer, &w, hit_color);
        }
    }

    u32 ht_count = Ecs::world_count(world, Ecs::COMPONENT_HURTBOX);
    for (u32 i = 0; i < ht_count; ++i)
    {
        Ecs::Entity e = Ecs::world_entity_at(world, Ecs::COMPONENT_HURTBOX, i);
        Ecs::Hurtbox *ht = (Ecs::Hurtbox *)Ecs::world_get_component(world, e, Ecs::COMPONENT_HURTBOX);
        Ecs::Transform wt;
        if (!ht || !Ecs::world_get_transform(world, e, &wt))
            continue;
        Collision::CollisionShape2D w;
        Systems::shape_to_world(&ht->shape, &wt, &w);
        debug_shape(renderer, &w, hurt_color);
    }

    u32 ar_count = Ecs::world_count(world, Ecs::COMPONENT_AREA);
    for (u32 i = 0; i < ar_count; ++i)
    {
        Ecs::Entity e = Ecs::world_entity_at(world, Ecs::COMPONENT_AREA, i);
        Ecs::Area *ar = (Ecs::Area *)Ecs::world_get_component(world, e, Ecs::COMPONENT_AREA);
        Ecs::Transform wt;
        if (!ar || !Ecs::world_get_transform(world, e, &wt))
            continue;
        Collision::CollisionShape2D w;
        Systems::shape_to_world(&ar->shape, &wt, &w);
        debug_shape(renderer, &w, area_color);
    }
}

/**
 * @brief Spawn the demo scene: a centered square and a spinning square.
 * @param out_spinner Receives the handle of the spinning entity.
 */
static void spawn_scene(Ecs::World *world, Ecs::Entity *out_spinner)
{
    Ecs::Entity entity;
    Ecs::Transform transform;
    Ecs::Sprite sprite;

    transform = Transform2D::make(Vec2::make(0.0f, 0.0f), 0.0f, Vec2::make(1.0f, 1.0f));
    sprite = Sprite::make(0, Vec2::make(140.0f, 140.0f), 1, 1);
    sprite.color = Color::make(0.85f, 0.30f, 0.30f, 1.0f);
    sprite.z = 0.0f; /* behind: lower z is drawn first */
    entity = Ecs::world_create_entity(world);
    Ecs::world_add_component(world, entity, Ecs::COMPONENT_TRANSFORM, &transform);
    Ecs::world_add_component(world, entity, Ecs::COMPONENT_SPRITE, &sprite);

    transform = Transform2D::make(Vec2::make(240.0f, 140.0f), 0.0f, Vec2::make(1.0f, 1.0f));
    sprite = Sprite::make(0, Vec2::make(80.0f, 80.0f), 1, 1);
    sprite.color = Color::make(0.30f, 0.85f, 0.35f, 1.0f);
    sprite.z = 1.0f;
    *out_spinner = Ecs::world_create_entity(world);
    Ecs::world_add_component(world, *out_spinner, Ecs::COMPONENT_TRANSFORM, &transform);
    Ecs::world_add_component(world, *out_spinner, Ecs::COMPONENT_SPRITE, &sprite);
}

/**
 * @brief Spawn the player: a root entity with sprite and hurtbox children
 *        parented to it.
 * @param out_player Receives the player root entity handle.
 */
static void spawn_player(Ecs::World *world, Ecs::Entity *out_player)
{
    Ecs::Entity root = Ecs::world_create_entity(world);
    Ecs::Transform root_t = Transform2D::make(Vec2::make(-260.0f, -120.0f), 0.0f, Vec2::make(1.0f, 1.0f));
    Ecs::world_add_component(world, root, Ecs::COMPONENT_TRANSFORM, &root_t);

    /* Sprite child: drawn at the player's position. */
    Ecs::Entity sprite_e = Ecs::world_create_entity(world);
    Ecs::Transform st = Transform2D::make(Vec2::make(0.0f, 0.0f), 0.0f, Vec2::make(1.0f, 1.0f));
    Ecs::Sprite sp = Sprite::make(0, Vec2::make(160.0f, 70.0f), 1, 1);
    sp.color = Color::make(0.30f, 0.45f, 0.90f, 1.0f);
    sp.z = 2.0f;
    Ecs::Parent sprite_parent;
    sprite_parent.parent = root;
    Ecs::world_add_component(world, sprite_e, Ecs::COMPONENT_TRANSFORM, &st);
    Ecs::world_add_component(world, sprite_e, Ecs::COMPONENT_SPRITE, &sp);
    Ecs::world_add_component(world, sprite_e, Ecs::COMPONENT_PARENT, &sprite_parent);

    /* Hurtbox child: collides at the player's position. */
    Ecs::Entity hurtbox_e = Ecs::world_create_entity(world);
    Ecs::Transform ht = Transform2D::make(Vec2::make(0.0f, 0.0f), 0.0f, Vec2::make(1.0f, 1.0f));
    Ecs::Hurtbox hb;
    memset(&hb, 0, sizeof(hb));
    Collision::make_rect(&hb.shape, Rect::make(-80.0f, -35.0f, 160.0f, 70.0f), 0.0f);
    hb.listeners[0] = log_hurt;
    hb.listener_count = 1;
    hb.mode = Ecs::TRIGGER_ON_ENTER;
    Ecs::Parent hurtbox_parent;
    hurtbox_parent.parent = root;
    Ecs::world_add_component(world, hurtbox_e, Ecs::COMPONENT_TRANSFORM, &ht);
    Ecs::world_add_component(world, hurtbox_e, Ecs::COMPONENT_HURTBOX, &hb);
    Ecs::world_add_component(world, hurtbox_e, Ecs::COMPONENT_PARENT, &hurtbox_parent);

    *out_player = root;
}

/**
 * @brief Spawn an enemy: a static entity with a sprite and a hitbox.
 * @param out_enemy Receives the enemy entity handle.
 */
static void spawn_enemy(Ecs::World *world, Ecs::Entity *out_enemy)
{
    Ecs::Entity enemy = Ecs::world_create_entity(world);
    Ecs::Transform transform = Transform2D::make(Vec2::make(260.0f, 120.0f), 0.0f, Vec2::make(1.0f, 1.0f));
    Ecs::Sprite sprite = Sprite::make(0, Vec2::make(120.0f, 120.0f), 1, 1);
    sprite.color = Color::make(0.80f, 0.30f, 0.70f, 1.0f);
    sprite.z = 2.0f;
    Ecs::world_add_component(world, enemy, Ecs::COMPONENT_TRANSFORM, &transform);
    Ecs::world_add_component(world, enemy, Ecs::COMPONENT_SPRITE, &sprite);

    Ecs::Hitbox hitbox;
    memset(&hitbox, 0, sizeof(hitbox));
    Collision::make_rect(&hitbox.shapes[0], Rect::make(-60.0f, -60.0f, 120.0f, 120.0f), 0.0f);
    hitbox.shape_count = 1;
    hitbox.listeners[0] = log_hit;
    hitbox.listener_count = 1;
    hitbox.mode = Ecs::TRIGGER_ON_ENTER;
    Ecs::world_add_component(world, enemy, Ecs::COMPONENT_HITBOX, &hitbox);

    *out_enemy = enemy;
}

/**
 * @brief Spawn `count` sprites in a grid to stress the renderer and the ECS.
 * @param world  World to spawn into.
 * @param count  Number of sprites to spawn.
 */
static void spawn_stress(Ecs::World *world, u32 count)
{
    u32 columns = (u32)sqrtf((f32)count);
    if (columns < 1u)
        columns = 1u;

    for (u32 i = 0; i < count; ++i)
    {

        f32 x = (f32)(i / columns) * 8.0f;
        f32 y = (f32)(i % columns) * 8.0f;
        static float zindex = 0.0f;
        Ecs::Transform transform = Transform2D::make(
            Vec2::make(x, y),
            (f32)i,
            Vec2::make(1.0f, 1.0f));

        Ecs::Sprite sprite = Sprite::make(
            0, Vec2::make((f32)((i >> 16) % 32u) + 8.0f, (f32)((i >> 8) % 32u) + 8.0f),
            1, 1);
        sprite.color = Color::make((f32)((i >> 16) & 0xFFu) / 255.0f,
                                   (f32)((i >> 8) & 0xFFu) / 255.0f,
                                   (f32)(i & 0xFFu) / 255.0f,
                                   1.0f);
        sprite.z = zindex;
        zindex += 0.0001f;

        Ecs::Entity entity = Ecs::world_create_entity(world);
        Ecs::world_add_component(world, entity, Ecs::COMPONENT_TRANSFORM, &transform);
        Ecs::world_add_component(world, entity, Ecs::COMPONENT_SPRITE, &sprite);
    }
}

/** A GPU texture loaded from the config, plus its world-space display size. */
struct ConfigTexture
{
    Texture::Texture gpu;
    f32 world_width;
    f32 world_height;
};

/**
 * @brief Load every texture listed in the config's `textures` sequence.
 * @param config Config document.
 * @param out    Destination array of textures (may be NULL when max == 0).
 * @param max    Capacity of `out`.
 * @return Number of texture entries processed (<= max).
 */
static u32 load_config_textures(Yaml::Node *config, ConfigTexture *out, u32 max)
{
    Yaml::Node list = (*config)["textures"];
    if (!list.IsSequence())
        return 0;

    u32 count = (u32)list.size();
    if (count > max)
        count = max;

    for (u32 i = 0; i < count; ++i)
    {
        out[i].gpu.id = 0;
        out[i].gpu.width = 0;
        out[i].gpu.height = 0;
        out[i].world_width = 0.0f;
        out[i].world_height = 0.0f;

        Yaml::Node entry = list[i];
        Yaml::TextureConfig cfg;
        if (!Yaml::load_texture(&entry, &cfg))
            continue;

        Texture::Settings settings = Texture::settings_default();
        settings.min_filter = (Texture::Filter)cfg.filter;
        settings.mag_filter = settings.min_filter;
        settings.wrap_s = (Texture::Wrap)cfg.repeat;
        settings.wrap_t = settings.wrap_s;

        Image::Image image;
        if (!Image::load(&image, cfg.path))
            continue;

        Texture::create(&out[i].gpu, image.pixels, image.width, image.height, &settings);
        Image::destroy(&image);

        out[i].world_width = cfg.size.x > 0.0f ? cfg.size.x : (f32)out[i].gpu.width;
        out[i].world_height = cfg.size.y > 0.0f ? cfg.size.y : (f32)out[i].gpu.height;
    }
    return count;
}

/**
 * @brief Spawn one entity per sprite listed in the config's `sprites` sequence.
 * @param world         World to spawn into.
 * @param config        Config document.
 * @param textures      Array of loaded textures (indexed by `texture_id`).
 * @param texture_count Number of loaded textures.
 * @param out_entities  Receives the spawned entity per sprite index (optional).
 * @param out_capacity  Capacity of `out_entities`.
 */
static void spawn_config_sprites(Ecs::World *world, Yaml::Node *config,
                                 const ConfigTexture *textures, u32 texture_count,
                                 Ecs::Entity *out_entities, u32 out_capacity)
{
    Yaml::Node list = (*config)["sprites"];
    if (!list.IsSequence())
        return;

    for (u32 i = 0; i < list.size(); ++i)
    {
        Yaml::Node entry = list[i];
        Yaml::SpriteConfig cfg;
        if (!Yaml::load_sprite(&entry, &cfg))
            continue;

        i32 texture = cfg.texture_id;
        if (texture < 0 || (u32)texture >= texture_count)
            texture = 0;

        const ConfigTexture *tex = &textures[texture];
        f32 tw = tex->world_width > 0.0f ? tex->world_width : (f32)tex->gpu.width;
        f32 th = tex->world_height > 0.0f ? tex->world_height : (f32)tex->gpu.height;
        if (tw <= 0.0f)
            tw = 32.0f;
        if (th <= 0.0f)
            th = 32.0f;

        Ecs::Transform transform = Transform2D::make(
            cfg.pos, 0.0f, Vec2::make(cfg.scale, cfg.scale));

        Ecs::Sprite sprite = Sprite::make(
            tex->gpu.id,
            Vec2::make(tw / (f32)cfg.h_frames, th / (f32)cfg.v_frames),
            cfg.h_frames, cfg.v_frames);

        Ecs::Entity entity = Ecs::world_create_entity(world);
        Ecs::world_add_component(world, entity, Ecs::COMPONENT_TRANSFORM, &transform);
        Ecs::world_add_component(world, entity, Ecs::COMPONENT_SPRITE, &sprite);

        if (out_entities && i < out_capacity)
            out_entities[i] = entity;
    }
}

/**
 * @brief Load every animation listed in the config's `animations` sequence.
 * @param config Config document.
 * @param out    Destination array of animations.
 * @param max    Capacity of `out`.
 * @return Number of animation entries loaded (<= max).
 */
static u32 load_config_animations(Yaml::Node *config, Animation::Animation *out, u32 max)
{
    Yaml::Node list = (*config)["animations"];
    if (!list.IsSequence())
        return 0;

    u32 count = (u32)list.size();
    if (count > max)
        count = max;

    for (u32 i = 0; i < count; ++i)
    {
        Animation::init(&out[i]);
        Yaml::Node entry = list[i];
        Yaml::load_animation(&entry, &out[i]);
    }
    return count;
}

/**
 * @brief Load every attack listed in the config's `attacks` sequence.
 * @param config Config document.
 * @param out    Destination array of attacks.
 * @param max    Capacity of `out`.
 * @return Number of attack entries loaded (<= max).
 */
static u32 load_config_attacks(Yaml::Node *config, Attack::Attack *out, u32 max)
{
    Yaml::Node list = (*config)["attacks"];
    if (!list.IsSequence())
        return 0;

    u32 count = (u32)list.size();
    if (count > max)
        count = max;

    for (u32 i = 0; i < count; ++i)
    {
        Attack::init(&out[i]);
        Yaml::Node entry = list[i];
        Yaml::load_attack(&entry, &out[i]);
    }
    return count;
}

/** @brief Logs a generic event fired through the central event bus. */
static void log_event(void *user, const char *event, void *payload)
{
    (void)user;
    (void)payload;
    fprintf(stderr, "[event] %s\n", event);
}

/** Central event-bus handler registry (name -> function). */
static const Event::Binding DEMO_BINDINGS[] = {
    {"log_event", log_event},
};

#define DEMO_BINDING_COUNT (sizeof(DEMO_BINDINGS) / sizeof(DEMO_BINDINGS[0]))

/** @brief Resolve a YAML handler name to a function, warning on unknown names. */
static Event::Handler resolve_handler(const char *name,
                                      const Event::Binding *bindings, u32 count)
{
    Event::Handler fn = Event::lookup(bindings, count, name);
    if (fn == NULL)
        fprintf(stderr, "warning: no handler registered for \"%s\"\n", name);
    return fn;
}

/**
 * @brief Wire the central event bus from the config's `subscribers` sequence.
 * @param config   Config document.
 * @param events   Central event bus.
 * @param bindings Handler name -> function table.
 * @param binding_count Number of entries in `bindings`.
 * @return Number of subscriptions added.
 */
static u32 load_config_subscribers(Yaml::Node *config, Event::Bus *events,
                                   const Event::Binding *bindings, u32 binding_count)
{
    Yaml::Node list = (*config)["subscribers"];
    if (!list.IsSequence())
        return 0;

    u32 added = 0;
    for (u32 i = 0; i < list.size(); ++i)
    {
        Yaml::Node entry = list[i];
        std::string event_name = Yaml::as_string(entry["event"], "");
        std::string handler_name = Yaml::as_string(entry["handler"], "");

        Event::Handler handler = resolve_handler(handler_name.c_str(), bindings, binding_count);
        if (handler == NULL)
            continue;

        Event::subscribe(events, event_name.c_str(), handler, NULL);
        ++added;
    }
    return added;
}

/**
 * @brief Update the demo scene: spin one entity and move the player from input.
 */
static void update_scene(Ecs::World *world, const Input::Input *input,
                         Ecs::Entity spinner, Ecs::Entity player, f32 dt)
{
    Ecs::Transform *t =
        (Ecs::Transform *)Ecs::world_get_component(world, spinner, Ecs::COMPONENT_TRANSFORM);
    if (t)
        t->rotation += 1.5f * dt;

    t = (Ecs::Transform *)Ecs::world_get_component(world, player, Ecs::COMPONENT_TRANSFORM);
    if (!t)
        return;

    f32 move_x = 0.0f;
    f32 move_y = 0.0f;

    if (Input::is_action_down(input, "move_left"))  move_x -= 1.0f;
    if (Input::is_action_down(input, "move_right")) move_x += 1.0f;
    if (Input::is_action_down(input, "move_up"))    move_y += 1.0f;
    if (Input::is_action_down(input, "move_down"))  move_y -= 1.0f;

    move_x += Input::gamepad_axis(input, GLFW_GAMEPAD_AXIS_LEFT_X);
    move_y += Input::gamepad_axis(input, GLFW_GAMEPAD_AXIS_LEFT_Y);

    t->position.x += move_x * 220.0f * dt;
    t->position.y += move_y * 220.0f * dt;
}

/** Context for the demo timer: flips the spinner's color every second. */
struct FlashTimerCtx
{
    Ecs::World *world;
    Ecs::Entity spinner;
    bool on;
};

/** @brief Timer callback: toggle the spinner between two colors. */
static void flash_spinner(void *user)
{
    FlashTimerCtx *ctx = (FlashTimerCtx *)user;

    Ecs::Sprite *sprite = (Ecs::Sprite *)Ecs::world_get_component(
        ctx->world, ctx->spinner, Ecs::COMPONENT_SPRITE);
    if (!sprite)
        return;

    ctx->on = !ctx->on;
    if (ctx->on)
        sprite->color = Color::make(0.30f, 0.85f, 0.35f, 1.0f);
    else
        sprite->color = Color::make(0.85f, 0.30f, 0.85f, 1.0f);
}

int main(int argc, char **argv)
{
    int max_frames = 0;
    int stress_count = 0;
    bool debug_boxes = false;
    parse_args(argc, argv, &max_frames, &stress_count, &debug_boxes);

    /* Window configuration (optional; falls back to built-in defaults). */
    Yaml::Node config;
    if (!Yaml::load(&config, "resources/config.yaml"))
        fprintf(stderr, "warning: failed to load resources/config.yaml, using defaults\n");

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        fprintf(stderr, "error: glfwInit failed\n");
        return 1;
    }

    Window::Options window_options;
    Yaml::Node window_node = config["window"];
    Yaml::load_window(&window_node, &window_options);

    Window::Window window;
    if (!Window::create(&window, &window_options))
    {
        fprintf(stderr, "error: glfwCreateWindow failed\n");
        glfwTerminate();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "error: gladLoadGLLoader failed\n");
        Window::destroy(&window);
        glfwTerminate();
        return 1;
    }

    fprintf(stderr, "OpenGL %s | %s\n",
            (const char *)glGetString(GL_VERSION),
            (const char *)glGetString(GL_RENDERER));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    /* Install our input callbacks before ImGui so ImGui chains to them. */
    Input::Input input;
    Input::init(&input, window.handle);

    /* Bind keys to named actions (the key-based API remains available too). */
    Input::Action actions[] = {
        {"move_left", {GLFW_KEY_LEFT, GLFW_KEY_A, -1, -1}},
        {"move_right", {GLFW_KEY_RIGHT, GLFW_KEY_D, -1, -1}},
        {"move_up", {GLFW_KEY_UP, GLFW_KEY_W, -1, -1}},
        {"move_down", {GLFW_KEY_DOWN, GLFW_KEY_S, -1, -1}},
        {"attack", {GLFW_KEY_SPACE, -1, -1, -1}},
    };
    Input::set_actions(&input, actions, 5);

    ImGui_ImplGlfw_InitForOpenGL(window.handle, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Ecs::World world;
    Ecs::world_init(&world, 256);

    Render::Renderer renderer;
    Render::renderer_init(&renderer);

    /* Load every texture listed in the config. */
    ConfigTexture config_textures[MAX_CONFIG_TEXTURES];
    u32 config_texture_count = load_config_textures(&config, config_textures,
                                                    MAX_CONFIG_TEXTURES);

    Render::Camera camera = Render::camera_make(
        Vec2::make(0.0f, 0.0f),
        Vec2::make((f32)window_options.width, (f32)window_options.height));

    Ecs::Entity spinner;
    Ecs::Entity player;
    Ecs::Entity enemy;
    spawn_scene(&world, &spinner);
    spawn_player(&world, &player);
    spawn_enemy(&world, &enemy);

    /* A lone trigger zone (demonstrates the Area component in the debug view). */
    Ecs::Entity zone = Ecs::world_create_entity(&world);
    Ecs::Transform zone_t = Transform2D::make(Vec2::make(0.0f, 260.0f), 0.0f, Vec2::make(1.0f, 1.0f));
    Ecs::Area zone_area;
    memset(&zone_area, 0, sizeof(zone_area));
    Collision::make_circle(&zone_area.shape, Vec2::make(0.0f, 0.0f), 40.0f);
    zone_area.listeners[0] = log_overlap;
    zone_area.listener_count = 1;
    zone_area.mode = Ecs::TRIGGER_ON_ENTER;
    Ecs::world_add_component(&world, zone, Ecs::COMPONENT_TRANSFORM, &zone_t);
    Ecs::world_add_component(&world, zone, Ecs::COMPONENT_AREA, &zone_area);

    Ecs::Entity sprite_entities[MAX_CONFIG_SPRITES];
    memset(sprite_entities, 0, sizeof(sprite_entities));
    spawn_config_sprites(&world, &config, config_textures, config_texture_count,
                         sprite_entities, MAX_CONFIG_SPRITES);

    /* Load animations and attacks from the config. */
    Animation::Animation animations[MAX_CONFIG_ANIMATIONS];
    u32 animation_count = load_config_animations(&config, animations, MAX_CONFIG_ANIMATIONS);

    Attack::Attack attacks[MAX_CONFIG_ATTACKS];
    u32 attack_count = load_config_attacks(&config, attacks, MAX_CONFIG_ATTACKS);

    Event::Bus event_bus;
    Event::init(&event_bus);

    /* Wire the central event bus from the config. */
    load_config_subscribers(&config, &event_bus, DEMO_BINDINGS, DEMO_BINDING_COUNT);

    Attack::Playback playback;
    memset(&playback, 0, sizeof(playback));

    if (stress_count > 0)
        spawn_stress(&world, (u32)stress_count);

    /* Timer system: flash the spinner's color once per second. */
    Timers::Manager timers;
    Timers::init(&timers, 16);

    FlashTimerCtx flash_ctx;
    flash_ctx.world = &world;
    flash_ctx.spinner = spinner;
    flash_ctx.on = false;
    Timers::start(&timers, 1.0f, true, flash_spinner, &flash_ctx);

    /* Audio playback manager (may fail without an audio device). */
    AudioManager::Manager audio;
    if (!AudioManager::init(&audio))
        fprintf(stderr, "warning: audio init failed\n");

    f64 last_time = glfwGetTime();
    int frame = 0;

    f32 anim_time = 0.0f;

    /* Fixed 60 Hz update step; rendering runs at the display refresh rate. */
    const f32 FIXED_DT = 1.0f / 60.0f;
    f32 accumulator = 0.0f;
    bool attack_queued = false;
    i32 attack_frame = 0;

    while (!Window::should_close(&window))
    {
        Input::begin_frame(&input);
        Window::poll_events();

        if (Input::is_key_pressed(&input, GLFW_KEY_ESCAPE))
            Window::set_should_close(&window, true);

        f64 now = glfwGetTime();
        f32 dt = (f32)(now - last_time);
        last_time = now;

        /* Clamp a long stall so it doesn't spiral into many catch-up steps. */
        if (dt > 0.25f)
            dt = 0.25f;
        accumulator += dt;

        if (attack_count > 0 && Input::is_action_pressed(&input, "attack"))
            attack_queued = true;

        /* Run zero or more fixed-step updates (60 Hz). */
        while (accumulator >= FIXED_DT)
        {
            update_scene(&world, &input, spinner, player, FIXED_DT);

            /* Resolve combat and trigger overlaps via the ECS system. */
            Systems::combat_system_update(&world);

            /* Advance the first animation on its target sprite. */
            anim_time += FIXED_DT;
            if (animation_count > 0)
            {
                const Animation::Animation *anim = &animations[0];
                i32 sprite_frame = Animation::frame_at(anim, anim_time);
                Ecs::Entity target = sprite_entities[anim->sprite_id];
                Ecs::Sprite *sp = (Ecs::Sprite *)Ecs::world_get_component(
                    &world, target, Ecs::COMPONENT_SPRITE);
                if (sp)
                    sp->frame = sprite_frame;
            }

            if (attack_queued)
            {
                Attack::playback_start(&playback, &attacks[0]);
                attack_frame = 0;
                attack_queued = false;
            }
            if (playback.attack)
            {
                attack_frame++;
                if (attack_count > 0 && attack_frame >= Attack::duration(&attacks[0]))
                    attack_frame = 0;
                Attack::playback_update(&playback, attack_frame, &event_bus);
            }

            Timers::update(&timers, FIXED_DT);

            accumulator -= FIXED_DT;
        }

        int fb_width = 0;
        int fb_height = 0;
        Window::framebuffer_size(&window, &fb_width, &fb_height);

        /* The camera follows the player (player is the camera's parent). */
        Ecs::Transform *player_t = (Ecs::Transform *)Ecs::world_get_component(
            &world, player, Ecs::COMPONENT_TRANSFORM);
        camera.position.x = player_t ? player_t->position.x : (f32)fb_width * 0.5f;
        camera.position.y = player_t ? player_t->position.y : (f32)fb_height * 0.5f;
        camera.size.x = (f32)fb_width;
        camera.size.y = (f32)fb_height;

        glViewport(0, 0, fb_width, fb_height);
        glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Render::renderer_begin(&renderer, &camera);
        Systems::render_system_draw(&world, &renderer);
        if (debug_boxes)
            draw_debug_world(&renderer, &world);
        Render::renderer_end(&renderer);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(260.0f, 130.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Engine", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("entities: %u", world.alive_count);
        ImGui::Text("sprites: %u", Ecs::world_count(&world, Ecs::COMPONENT_SPRITE));
        ImGui::Text("quads: %u", renderer.total_quads);
        ImGui::Text("frame: %d", frame);
        ImGui::Text("dt: %.3f ms", dt * 1000.0f);
        ImGui::Text("mouse: %.0f, %.0f", Input::mouse_x(&input), Input::mouse_y(&input));
        ImGui::Text("gamepad: %s", Input::gamepad_connected(&input) ? "yes" : "no");
        ImGui::Checkbox("debug boxes", &debug_boxes);
        if (ImGui::SliderInt("stress: %d", &stress_count, 0, 1000000))
        {
            /* Re-spawn the scene, config sprites and stress grid. */
            Ecs::world_destroy(&world);
            Ecs::world_init(&world, 256);
            spawn_scene(&world, &spinner);
            spawn_player(&world, &player);
            spawn_enemy(&world, &enemy);
            flash_ctx.spinner = spinner;
            spawn_config_sprites(&world, &config, config_textures, config_texture_count,
                                 sprite_entities, MAX_CONFIG_SPRITES);
            anim_time = 0.0f;
            if (stress_count > 0)
                spawn_stress(&world, (u32)stress_count);
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        Window::swap_buffers(&window);

        if (max_frames > 0 && ++frame >= max_frames)
            break;
    }

    u32 final_sprites = Ecs::world_count(&world, Ecs::COMPONENT_SPRITE);
    u32 final_quads = renderer.total_quads;

    Render::renderer_shutdown(&renderer);
    for (u32 i = 0; i < config_texture_count; ++i)
        Texture::destroy(&config_textures[i].gpu);
    for (u32 i = 0; i < animation_count; ++i)
        Animation::destroy(&animations[i]);
    for (u32 i = 0; i < attack_count; ++i)
        Attack::destroy(&attacks[i]);
    Event::destroy(&event_bus);
    Ecs::world_destroy(&world);

    AudioManager::shutdown(&audio);
    Timers::destroy(&timers);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Input::shutdown(&input);

    Window::destroy(&window);
    glfwTerminate();

    fprintf(stderr, "clean exit (rendered %d frames, sprites=%u, quads=%u)\n",
            frame, final_sprites, final_quads);
    return 0;
}