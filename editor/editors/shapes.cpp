/**
 * @file shapes.cpp
 * @brief Collision-shape (hitbox) editor helpers.
 */
#include "editors/shapes.hpp"
#include "config/config_helpers.hpp"
#include "services/errors.hpp"
#include "services/notifications.hpp"
#include "services/undo.hpp"

#include "gameplay/collision/collision_shape2d.hpp"
#include "serialization/collision_shape2d_yaml.hpp"
#include "serialization/yaml.hpp"

#include <string.h>

using namespace Engine;

namespace Editor {

/** @brief Fill a CollisionShape2D from a ShapeEntry. */
static void entry_to_shape(const ShapeEntry* entry, Collision::CollisionShape2D* out)
{
    Collision::init(out);
    if (entry->type == 1)
    {
        Rect::Rect r = Rect::make(entry->pos.x, entry->pos.y,
                                  entry->size.x, entry->size.y);
        Collision::make_rect(out, r, entry->rotation);
    }
    else if (entry->type == 2)
    {
        u32 count = entry->point_count < 3 ? 3u : (u32)entry->point_count;
        Vec2::Vec2 pts[EDITOR_SHAPE_MAX_POINTS];
        for (u32 i = 0; i < count; ++i)
            pts[i] = entry->points[i];
        Collision::make_polygon(out, pts, count);
    }
    else
    {
        Collision::make_circle(out, entry->center, entry->radius);
    }
}

void load_shapes(Editor* editor, const char* config_path)
{
    editor->shapes.shape_count = 0;

    Yaml::Node root;
    if (!Yaml::load(&root, config_path))
        return;

    Yaml::Node shapes = root["collision_shapes"];
    if (!shapes.IsSequence())
        return;

    for (usize i = 0; i < shapes.size() && editor->shapes.shape_count < EDITOR_MAX_SHAPES; ++i)
    {
        Collision::CollisionShape2D shape;
        Collision::init(&shape);

        Yaml::Node node = shapes[i];
        if (!Yaml::load_collision_shape(&node, &shape))
            continue;

        ShapeEntry* entry = &editor->shapes.shapes[editor->shapes.shape_count];
        entry->yaml_index = (i32)i;
        entry->type = shape.type;
        if (shape.type == Collision::SHAPE_RECT)
        {
            entry->pos = shape.rect.position;
            entry->size = shape.rect.size;
            entry->rotation = shape.rotation;
        }
        else if (shape.type == Collision::SHAPE_POLYGON)
        {
            entry->point_count = (i32)shape.point_count;
            for (i32 p = 0; p < entry->point_count; ++p)
                entry->points[p] = shape.points[p];
        }
        else
        {
            entry->center = shape.pos;
            entry->radius = shape.radius;
        }
        editor->shapes.shape_count++;
    }
}

void save_shapes(Editor* editor, const char* config_path)
{
    Yaml::Node root = config_open(config_path);
    Yaml::Node out(YAML::NodeType::Sequence);

    for (i32 s = 0; s < editor->shapes.shape_count; ++s)
    {
        Collision::CollisionShape2D shape;
        entry_to_shape(&editor->shapes.shapes[s], &shape);
        out.push_back(Yaml::make_collision_shape(&shape));
    }

    root["collision_shapes"] = out;

    if (Yaml::save(&root, config_path))
    {
        notify(editor, "saved shapes");
        load_shapes(editor, config_path);
    }
    else
    {
        error_raise(editor, "save failed");
    }
}

void delete_shape_at(Editor* editor, const char* config_path, i32 yaml_index)
{
    Yaml::Node root = config_open(config_path);
    Yaml::Node shapes = root["collision_shapes"];
    if (!shapes.IsSequence() || yaml_index < 0 || (usize)yaml_index >= shapes.size())
    {
        error_raise(editor, "shape not found");
        return;
    }

    root["collision_shapes"] = config_remove_index(shapes, yaml_index);

    if (Yaml::save(&root, config_path))
    {
        notify(editor, "deleted shape");
        load_shapes(editor, config_path);
        editor->shapes.selected_shape = -1;
    }
    else
    {
        error_raise(editor, "save failed");
    }
}

void shape_new(Editor* editor)
{
    if (editor->shapes.shape_count >= EDITOR_MAX_SHAPES)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    ShapeEntry* sh = &editor->shapes.shapes[editor->shapes.shape_count];
    sh->yaml_index = editor->shapes.shape_count;
    sh->type = 0;
    sh->center = Vec2::make(0.0f, 0.0f);
    sh->radius = 16.0f;
    sh->pos = Vec2::make(-16.0f, -16.0f);
    sh->size = Vec2::make(32.0f, 32.0f);
    sh->rotation = 0.0f;
    sh->point_count = 0;

    editor->shapes.selected_shape = editor->shapes.shape_count;
    editor->shapes.shape_count++;
}

i32 shape_duplicate(Editor* editor, i32 index)
{
    if (index < 0 || index >= editor->shapes.shape_count || editor->shapes.shape_count >= EDITOR_MAX_SHAPES)
        return -1;

    ShapeEntry* dst = &editor->shapes.shapes[editor->shapes.shape_count];
    *dst = editor->shapes.shapes[index];
    dst->yaml_index = editor->shapes.shape_count;
    editor->shapes.shape_count++;
    return dst->yaml_index;
}

void shape_select(Editor* editor, i32 index)
{
    if (index < 0 || index >= editor->shapes.shape_count)
        return;
    editor->shapes.selected_shape = index;
}

} // namespace Editor
