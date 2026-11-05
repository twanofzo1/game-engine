/**
 * @file editor.hpp
 * @brief Editor state and helpers shared by the editor's UI panels.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "graphics/rendering/texture.hpp"

/** Maximum number of files shown in the asset tree. */
#define EDITOR_MAX_FILES 256

/** Maximum number of saved sprites shown in the sprite list. */
#define EDITOR_MAX_SPRITES 64

/** Maximum length of a file path shown in the editor. */
#define EDITOR_MAX_PATH 256

/** Maximum length of a sprite name, including the null terminator. */
#define EDITOR_SPRITE_NAME_MAX 64

/** Maximum number of animations shown in the editor. */
#define EDITOR_MAX_ANIMATIONS 128

/** Maximum number of keyframes per animation. */
#define EDITOR_MAX_KEYFRAMES 128

/** Maximum length of an animation name, including the null terminator. */
#define EDITOR_ANIMATION_NAME_MAX 32

/** Maximum number of queued notifications. */
#define EDITOR_MAX_NOTIFICATIONS 8

/** Maximum number of attacks shown in the editor. */
#define EDITOR_MAX_ATTACKS 64

/** Maximum number of event keyframes per attack. */
#define EDITOR_MAX_ATTACK_EVENTS 64

/** Maximum number of hitbox keyframes per attack. */
#define EDITOR_MAX_ATTACK_HITBOXES 64

/** Maximum number of movement keyframes per attack. */
#define EDITOR_MAX_ATTACK_MOVEMENTS 64

/** Maximum number of collision shapes shown in the editor. */
#define EDITOR_MAX_SHAPES 64

/** Maximum length of an attack event name. */
#define EDITOR_EVENT_NAME_MAX 32

/** Maximum polygon vertices in a shape being edited. */
#define EDITOR_SHAPE_MAX_POINTS 16

namespace Editor
{

    /** Undo/redo stack (defined in undo.hpp). */
    struct UndoState;

    /** One file entry in the asset tree. */
    struct FileEntry
    {
        char name[EDITOR_MAX_PATH]; ///< File name (no directory).
    };

    /** One saved sprite in the config's `sprites` list. */
    struct SpriteEntry
    {
        char name[EDITOR_SPRITE_NAME_MAX]; ///< Sprite name ("" = unnamed).
        char path[EDITOR_MAX_PATH];        ///< Texture path this sprite samples.
        i32 frames_x;                      ///< Horizontal frame count.
        i32 frames_y;                      ///< Vertical frame count.
        i32 yaml_index;                    ///< Index into the config `sprites` sequence.
    };

    /** One keyframe in an animation being edited. */
    struct KeyframeEntry
    {
        f32 time;  ///< Time in seconds from the animation start.
        i32 frame; ///< Sprite frame to show.
    };

    /** One animation read from the config. */
    struct AnimationEntry
    {
        char name[EDITOR_ANIMATION_NAME_MAX];          ///< Animation name.
        i32 sprite_id;                                 ///< Sprite this drives.
        KeyframeEntry keyframes[EDITOR_MAX_KEYFRAMES]; ///< Keyframes.
        i32 keyframe_count;
        i32 yaml_index; ///< Index into the config `animations` sequence.
    };

    /** One event keyframe in an attack. */
    struct AttackEventKey
    {
        i32 frame;                        ///< Frame index.
        char name[EDITOR_EVENT_NAME_MAX]; ///< Event name.
    };

    /** One hitbox keyframe in an attack (references a collision shape). */
    struct AttackHitboxKey
    {
        i32 frame;    ///< Frame index.
        i32 shape_id; ///< Index into the config `collision_shapes` sequence, or -1.
    };

    /** One movement keyframe in an attack. */
    struct AttackMovementKey
    {
        i32 frame;                 ///< Frame index.
        Engine::Vec2::Vec2 vector; ///< Movement vector.
    };

    /** One attack read from the config. */
    struct AttackEntry
    {
        char name[EDITOR_SPRITE_NAME_MAX];               ///< Attack name.
        i32 animation_id;                                ///< Animation this attack plays.
        i32 windup;                                      ///< Windup phase length in frames.
        i32 active;                                      ///< Active phase length in frames.
        i32 recovery;                                    ///< Recovery phase length in frames.
        AttackEventKey events[EDITOR_MAX_ATTACK_EVENTS]; ///< Event keys.
        i32 event_count;
        AttackHitboxKey hitboxes[EDITOR_MAX_ATTACK_HITBOXES]; ///< Hitbox keys.
        i32 hitbox_count;
        AttackMovementKey movements[EDITOR_MAX_ATTACK_MOVEMENTS]; ///< Movement keys.
        i32 movement_count;
        i32 yaml_index; ///< Index into the config `attacks` sequence.
    };

    /** One collision shape (hitbox) read from the config. */
    struct ShapeEntry
    {
        i32 type; ///< 0 = circle, 1 = rect, 2 = polygon.
        Engine::Vec2::Vec2 center;
        f32 radius; ///< Circle.
        Engine::Vec2::Vec2 pos;
        Engine::Vec2::Vec2 size;
        f32 rotation;                                       ///< Rect.
        Engine::Vec2::Vec2 points[EDITOR_SHAPE_MAX_POINTS]; ///< Polygon.
        i32 point_count;
        i32 yaml_index; ///< Index into the config `collision_shapes` sequence.
    };

    /** Editor state: what the editor is currently focused on. */
    enum State
    {
        STATE_IDLE = 0,      ///< Nothing selected.
        STATE_SPRITE = 1,    ///< Editing a sprite or image.
        STATE_ANIMATION = 2, ///< Editing an animation.
        STATE_ATTACK = 3     ///< Editing an attack.
    };

    /** A switch action deferred by the unsaved-changes guard. */
    enum PendingAction
    {
        PENDING_NONE = 0,
        PENDING_OPEN_IMAGE = 1,
        PENDING_OPEN_SPRITE = 2,
        PENDING_OPEN_ANIMATION = 3,
        PENDING_NEW_ANIMATION = 4,
        PENDING_OPEN_ATTACK = 5,
        PENDING_NEW_ATTACK = 6,
    };

    /** Asset tree: files scanned from the asset directory. */
    struct Assets
    {
        FileEntry files[EDITOR_MAX_FILES]; ///< Files in the asset directory.
        i32 file_count;                    ///< Number of files.
        i32 selected_file;                 ///< Index into `files`, or -1.
        char directory[EDITOR_MAX_PATH];   ///< Scanned asset directory.
    };

    /** Sprite preview / viewport state. */
    struct Preview
    {
        Engine::Texture::Texture gpu;       ///< Preview texture (id 0 when none loaded).
        bool preview_loaded;                ///< True when `gpu` holds a texture.
        char preview_path[EDITOR_MAX_PATH]; ///< Path of the loaded image.
        i32 frames_x;                       ///< Horizontal frame count.
        i32 frames_y;                       ///< Vertical frame count.
        i32 frame;                          ///< Current frame (row-major).
        bool nearest;                       ///< Nearest-neighbor filtering when true.
        f32 zoom;                           ///< Preview zoom multiplier (1 = fit).
        bool single_frame;                  ///< Show only the selected frame when true.
    };

    /** Saved sprites. */
    struct Sprites
    {
        SpriteEntry sprites[EDITOR_MAX_SPRITES];  ///< Sprites read from config.
        i32 sprite_count;                         ///< Number of sprites.
        i32 selected_sprite;                      ///< Index into `sprites`, or -1.
        char sprite_name[EDITOR_SPRITE_NAME_MAX]; ///< Name for the next save.
    };

    /** Error popup. */
    struct Errors
    {
        bool error_active;
        char error_message[EDITOR_MAX_PATH];
    };

    /** Notification toasts (ring buffer). */
    struct Notifications
    {
        char notifications[EDITOR_MAX_NOTIFICATIONS][EDITOR_MAX_PATH];
        i32 notification_count;
        i32 notification_head;
    };

    /** Unsaved-changes guard. */
    struct Guard
    {
        bool dirty;
        PendingAction pending_action;
        i32 pending_index;
        char pending_path[EDITOR_MAX_PATH];
    };

    /** Animation editor state. */
    struct AnimationEditor
    {
        AnimationEntry animations[EDITOR_MAX_ANIMATIONS]; ///< Animations from config.
        i32 animation_count;                              ///< Number of animations.
        i32 selected_animation;                           ///< Index, or -1.

        i32 anim_sprite_id;                                 ///< Sprite the edited animation drives.
        i32 anim_frame;                                     ///< Frame for the next keyframe.
        f32 anim_time;                                      ///< Timestamp for the next keyframe.
        char anim_name[EDITOR_ANIMATION_NAME_MAX];          ///< Name being edited.
        KeyframeEntry anim_keyframes[EDITOR_MAX_KEYFRAMES]; ///< Keyframes being edited.
        i32 anim_keyframe_count;
        bool anim_playing;  ///< Timeline playback state.
        f32 anim_play_time; ///< Timeline playhead in seconds.
        f32 anim_speed;     ///< Playback speed multiplier.
        f32 anim_time_step; ///< Time increment per added keyframe.
    };

    /** Attack editor state. */
    struct AttackEditor
    {
        AttackEntry attacks[EDITOR_MAX_ATTACKS]; ///< Attacks from config.
        i32 attack_count;                        ///< Number of attacks.
        i32 selected_attack;                     ///< Index, or -1.

        i32 attack_animation_id;                  ///< Animation the edited attack plays.
        i32 attack_windup;                        ///< Windup phase length in frames.
        i32 attack_active;                        ///< Active phase length in frames.
        i32 attack_recovery;                      ///< Recovery phase length in frames.
        char attack_name[EDITOR_SPRITE_NAME_MAX]; ///< Name being edited.

        AttackEventKey attack_events[EDITOR_MAX_ATTACK_EVENTS]; ///< Events.
        i32 attack_event_count;
        AttackHitboxKey attack_hitboxes[EDITOR_MAX_ATTACK_HITBOXES]; ///< Hitbox keys.
        i32 attack_hitbox_count;
        AttackMovementKey attack_movements[EDITOR_MAX_ATTACK_MOVEMENTS]; ///< Movement keys.
        i32 attack_movement_count;

        char attack_event_name[EDITOR_EVENT_NAME_MAX]; ///< Name for next event.
        i32 attack_hitbox_shape;                       ///< Shape for next hitbox key.
        Engine::Vec2::Vec2 attack_move;                ///< Vector for next movement key.

        bool attack_playing;   ///< Timeline playback state.
        i32 attack_play_frame; ///< Timeline playhead frame.
        f32 attack_play_accum; ///< Fractional frame accumulator for playback.
        f32 attack_speed;      ///< Playback speed multiplier.

        bool timeline_show_movement; ///< Show the movement timeline lane.
        bool timeline_show_hitbox;   ///< Show the hitbox timeline lane.
        bool timeline_show_frame;    ///< Show the frame timeline lane.
    };

    /** Hitbox (collision shape) editor. */
    struct ShapeEditor
    {
        ShapeEntry shapes[EDITOR_MAX_SHAPES]; ///< Shapes from config.
        i32 shape_count;
        i32 selected_shape; ///< Index into `shapes`, or -1.
    };

    /** Viewport direct-editing drag state. */
    struct ViewportEdit
    {
        i32 view_edit_drag;     ///< Which handle is dragged (0 = none).
        i32 view_edit_drag_sub; ///< Extra data (corner / point index).
    };

    /** Rename popup state. */
    struct Rename
    {
        bool rename_open;                         ///< Request to open the rename popup.
        bool rename_is_animation;                 ///< Renaming an animation.
        bool rename_is_attack;                    ///< Renaming an attack.
        i32 rename_index;                         ///< YAML index of the item being renamed.
        char rename_name[EDITOR_SPRITE_NAME_MAX]; ///< New name buffer.
    };

    /** Editor state: asset tree, preview, sprites and the per-asset editors. */
    struct Editor
    {
        Assets assets;
        Preview preview;
        Sprites sprites;
        Errors errors;
        Notifications notifications;
        Guard guard;
        AnimationEditor animation;
        AttackEditor attack;
        ShapeEditor shapes;
        ViewportEdit viewport;
        Rename rename;

        char config_path[EDITOR_MAX_PATH]; ///< Path to the YAML config file.
        UndoState *undo;                   ///< Undo/redo stack (owned by main).
        State state;                       ///< Current editor state.
    };

    /** @brief Reset the editor to an empty state. */
    void init(Editor *editor);

    /** @brief Transition the editor to `state` (state machine entry point). */
    void set_state(Editor *editor, State state);

    /** @brief Free the editor's GPU resources. */
    void destroy(Editor *editor);

    /** @brief List image files under `directory` into the editor's file tree. */
    void scan_directory(Editor *editor, const char *directory);

    /** @brief Load `path` as the preview texture (frees the previous one). */
    void load_preview(Editor *editor, const char *path);

    /** @brief Open a saved sprite (texture + frame grid) into the preview. */
    void open_sprite(Editor *editor, const SpriteEntry *entry);

    /** @brief Reload the saved-sprite list from the config file. */
    void load_sprites(Editor *editor, const char *config_path);

    /** @brief Append (or overwrite by name) the current preview as a sprite. */
    void save_sprite(Editor *editor, const char *config_path);

    /** @brief Remove the sprite at `yaml_index` from the config file. */
    void delete_sprite_at(Editor *editor, const char *config_path, i32 yaml_index);

    /** @brief Rename the sprite at `yaml_index`. */
    void rename_sprite(Editor *editor, const char *config_path, i32 yaml_index,
                       const char *new_name);

} // namespace Editor
