/**
 * @file window.cpp
 * @brief YAML serialization for Window::Options, built on the Yaml helpers.
 */
#include "serialization/window_yaml.hpp"

#include <string.h>

namespace Yaml {

bool load_window(const Node* node, Engine::Window::Options* out)
{
    if (out == NULL)
        return false;

    *out = Engine::Window::options_default();
    if (node == NULL || !node->IsMap())
        return false;

    Engine::Vec2::Vec2 size = as_vec2(
        (*node)["size"],
        Engine::Vec2::make(
            (f32)out->width, 
            (f32)out->height
        ));

    Engine::Vec2::Vec2 pos  = as_vec2(
        (*node)["position"],
        Engine::Vec2::make(
            (f32)out->position_x, 
            (f32)out->position_y
        ));

    out->width       = (i32)size.x;
    out->height      = (i32)size.y;
    out->position_x  = (i32)pos.x;
    out->position_y  = (i32)pos.y;
    out->vsync       = as_bool((*node)["vsync"], out->vsync);
    out->screen_mode = as_int((*node)["screen_mode"], out->screen_mode);
    out->resizable   = as_bool((*node)["resizable"], out->resizable);
    out->borderless  = as_bool((*node)["borderless"], out->borderless);

    std::string title = as_string((*node)["title"], out->title);
    strncpy(out->title, title.c_str(), WINDOW_TITLE_MAX - 1);
    out->title[WINDOW_TITLE_MAX - 1] = '\0';

    return true;
}

Node make_window(const Engine::Window::Options* options)
{
    Node node;

    node["size"]        = make_vec2(Engine::Vec2::make((f32)options->width, (f32)options->height));
    node["position"]    = make_vec2(Engine::Vec2::make((f32)options->position_x, (f32)options->position_y));
    node["vsync"]       = options->vsync;
    node["screen_mode"] = options->screen_mode;
    node["resizable"]   = options->resizable;
    node["borderless"]  = options->borderless;
    node["title"]       = std::string(options->title);

    return node;
}

} // namespace Yaml
