/**
 * @file editor.cpp
 * @brief Editor state helpers: directory scanning and sprite preview loading.
 */
#include "core/editor.hpp"
#include "config/config_helpers.hpp"
#include "services/errors.hpp"
#include "services/notifications.hpp"

#include "resources/image.hpp"
#include "serialization/sprite_yaml.hpp"
#include "serialization/texture_yaml.hpp"
#include "serialization/yaml.hpp"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace Engine; /* usage code, like src/main.cpp */

namespace Editor
{

    /** @brief Case-insensitive suffix match. */
    static bool ends_with(const char *str, const char *suffix)
    {
        usize sl = strlen(str);
        usize su = strlen(suffix);
        if (sl < su)
            return false;

        const char *tail = str + sl - su;
        for (usize i = 0; i < su; ++i)
        {
            if (tolower((unsigned char)tail[i]) != tolower((unsigned char)suffix[i]))
                return false;
        }
        return true;
    }

    /** @brief True when `name` has a supported image extension. */
    static bool is_image_file(const char *name)
    {
        return ends_with(name, ".png") || ends_with(name, ".jpg") ||
               ends_with(name, ".jpeg") || ends_with(name, ".bmp") ||
               ends_with(name, ".tga") || ends_with(name, ".hdr") ||
               ends_with(name, ".gif");
    }

    void set_state(Editor *editor, State state)
    {
        editor->state = state;
    }

    void init(Editor *editor)
    {
        memset(editor, 0, sizeof(*editor));
        editor->assets.selected_file = -1;
        editor->sprites.selected_sprite = -1;
        editor->animation.selected_animation = -1;
        editor->attack.selected_attack = -1;
        editor->animation.anim_sprite_id = -1;
        editor->animation.anim_speed = 1.0f;
        editor->animation.anim_time_step = 0.1f;
        editor->state = STATE_SPRITE;
        editor->preview.frames_x = 1;
        editor->preview.frames_y = 1;
        editor->preview.zoom = 1.0f;
        editor->preview.nearest = true;

        editor->attack.attack_animation_id = -1;
        editor->attack.attack_speed = 1.0f;
        editor->attack.timeline_show_movement = true;
        editor->attack.timeline_show_hitbox = true;
        editor->attack.timeline_show_frame = true;

        editor->shapes.selected_shape = -1;
    }

    void destroy(Editor *editor)
    {
        if (editor->preview.preview_loaded)
        {
            Texture::destroy(&editor->preview.gpu);
            editor->preview.preview_loaded = false;
        }
    }

    void scan_directory(Editor *editor, const char *directory)
    {
        editor->assets.file_count = 0;
        editor->assets.selected_file = -1;
        strncpy(editor->assets.directory, directory, EDITOR_MAX_PATH - 1);
        editor->assets.directory[EDITOR_MAX_PATH - 1] = '\0';

        DIR *dir = opendir(directory);
        if (!dir)
            return;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL && editor->assets.file_count < EDITOR_MAX_FILES)
        {
            if (!is_image_file(entry->d_name))
                continue;

            snprintf(editor->assets.files[editor->assets.file_count].name, EDITOR_MAX_PATH, "%s",
                     entry->d_name);
            editor->assets.file_count++;
        }
        closedir(dir);
    }

    void load_preview(Editor *editor, const char *path)
    {
        if (editor->preview.preview_loaded)
        {
            Texture::destroy(&editor->preview.gpu);
            editor->preview.preview_loaded = false;
        }

        Image::Image image;
        if (!Image::load(&image, path))
            return;

        Texture::Settings settings = Texture::settings_default();
        if (editor->preview.nearest)
        {
            settings.min_filter = Texture::FILTER_NEAREST;
            settings.mag_filter = Texture::FILTER_NEAREST;
        }

        if (Texture::create(&editor->preview.gpu, image.pixels, image.width, image.height,
                            &settings))
        {
            editor->preview.preview_loaded = true;
            editor->state = STATE_SPRITE;
            editor->preview.frames_x = 1;
            editor->preview.frames_y = 1;
            editor->preview.frame = 0;
            editor->preview.zoom = 1.0f; /* re-fit the new image */
            strncpy(editor->preview.preview_path, path, EDITOR_MAX_PATH - 1);
            editor->preview.preview_path[EDITOR_MAX_PATH - 1] = '\0';
        }
        Image::destroy(&image);
    }

    void open_sprite(Editor *editor, const SpriteEntry *entry)
    {
        if (entry->path[0] == '\0')
            return;

        load_preview(editor, entry->path);
        if (editor->preview.preview_loaded)
        {
            editor->preview.frames_x = entry->frames_x;
            editor->preview.frames_y = entry->frames_y;
            editor->preview.frame = 0;
            snprintf(editor->sprites.sprite_name, EDITOR_SPRITE_NAME_MAX, "%s", entry->name);
            editor->animation.anim_sprite_id = entry->yaml_index;
        }
    }

    void load_sprites(Editor *editor, const char *config_path)
    {
        editor->sprites.sprite_count = 0;

        Yaml::Node root;
        if (!Yaml::load(&root, config_path))
            return;

        Yaml::Node sprites = root["sprites"];
        Yaml::Node textures = root["textures"];
        if (!sprites.IsSequence())
            return;

        for (usize i = 0; i < sprites.size() && editor->sprites.sprite_count < EDITOR_MAX_SPRITES; ++i)
        {
            Yaml::SpriteConfig scfg;
            Yaml::Node sprite_node = sprites[i];
            if (!Yaml::load_sprite(&sprite_node, &scfg))
                continue;

            SpriteEntry *entry = &editor->sprites.sprites[editor->sprites.sprite_count];
            entry->yaml_index = (i32)i;
            snprintf(entry->name, EDITOR_SPRITE_NAME_MAX, "%s", scfg.name);
            entry->frames_x = scfg.h_frames;
            entry->frames_y = scfg.v_frames;
            entry->path[0] = '\0';

            if (textures.IsSequence() && scfg.texture_id >= 0 &&
                (usize)scfg.texture_id < textures.size())
            {
                Yaml::TextureConfig tcfg;
                Yaml::Node tex_node = textures[scfg.texture_id];
                if (Yaml::load_texture(&tex_node, &tcfg))
                    snprintf(entry->path, EDITOR_MAX_PATH, "%s", tcfg.path);
            }
            editor->sprites.sprite_count++;
        }
    }

    /** @brief Index of the texture whose path equals `path`, or -1. */
    static i32 find_texture_by_path(const Yaml::Node &textures, const char *path)
    {
        for (usize i = 0; i < textures.size(); ++i)
        {
            Yaml::TextureConfig tcfg;
            Yaml::Node tex_node = textures[i];
            if (Yaml::load_texture(&tex_node, &tcfg) && strcmp(tcfg.path, path) == 0)
                return (i32)i;
        }
        return -1;
    }

    /** @brief Append the preview texture to the config; return its id. */
    static i32 append_texture(Editor *editor, Yaml::Node &textures)
    {
        i32 id = (i32)textures.size();

        Yaml::TextureConfig tcfg;
        snprintf(tcfg.path, TEXTURE_PATH_MAX, "%s", editor->preview.preview_path);
        tcfg.filter = editor->preview.nearest ? 0 : 1;
        tcfg.repeat = 0;
        tcfg.size = Vec2::make(0.0f, 0.0f);
        textures.push_back(Yaml::make_texture(&tcfg));
        return id;
    }

    void save_sprite(Editor *editor, const char *config_path)
    {
        if (!editor->preview.preview_loaded)
        {
            error_raise(editor, "no image loaded");
            return;
        }
        if (editor->sprites.sprite_name[0] == '\0')
        {
            error_raise(editor, "name required");
            return;
        }

        Yaml::Node root = config_open(config_path);
        Yaml::Node textures = config_sequence(&root, "textures");

        i32 texture_id = find_texture_by_path(textures, editor->preview.preview_path);
        if (texture_id < 0)
            texture_id = append_texture(editor, textures);

        Yaml::SpriteConfig scfg;
        snprintf(scfg.name, SPRITE_NAME_MAX, "%s", editor->sprites.sprite_name);
        scfg.texture_id = texture_id;
        scfg.h_frames = editor->preview.frames_x;
        scfg.v_frames = editor->preview.frames_y;
        scfg.pos = Vec2::make(0.0f, 0.0f);
        scfg.scale = 1.0f;

        bool replaced = config_upsert_by_name(&root, "sprites",
                                              Yaml::make_sprite(&scfg),
                                              editor->sprites.sprite_name, -1);

        if (Yaml::save(&root, config_path))
        {
            notify(editor, replaced ? "updated sprite" : "saved sprite");
            editor->guard.dirty = false;
            load_sprites(editor, config_path);
        }
        else
        {
            error_raise(editor, "save failed");
        }
    }

    void delete_sprite_at(Editor *editor, const char *config_path, i32 yaml_index)
    {
        Yaml::Node root = config_open(config_path);
        Yaml::Node sprites = root["sprites"];
        if (!sprites.IsSequence() || yaml_index < 0 || (usize)yaml_index >= sprites.size())
        {
            error_raise(editor, "sprite not found");
            return;
        }

        root["sprites"] = config_remove_index(sprites, yaml_index);

        if (Yaml::save(&root, config_path))
        {
            notify(editor, "deleted sprite");
            load_sprites(editor, config_path);
            editor->sprites.selected_sprite = -1;
        }
        else
        {
            error_raise(editor, "save failed");
        }
    }

    void rename_sprite(Editor *editor, const char *config_path, i32 yaml_index,
                       const char *new_name)
    {
        if (new_name[0] == '\0')
        {
            error_raise(editor, "name required");
            return;
        }

        Yaml::Node root = config_open(config_path);
        Yaml::Node sprites = root["sprites"];
        if (!sprites.IsSequence() || yaml_index < 0 || (usize)yaml_index >= sprites.size())
        {
            error_raise(editor, "sprite not found");
            return;
        }

        config_rename_at(&root, "sprites", yaml_index, new_name);

        if (Yaml::save(&root, config_path))
        {
            notify(editor, "renamed sprite");
            load_sprites(editor, config_path);
        }
        else
        {
            error_raise(editor, "save failed");
        }
    }

} // namespace Editor
