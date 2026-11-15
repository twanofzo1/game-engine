/**
 * @file audio.cpp
 * @brief Sound handling system implementation (miniaudio backend).
 */
#include "common/audio/audio.hpp"
#include "common/log/log.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stddef.h>

/** @brief A single loaded sound slot in the fixed pool. */
struct SoundSlot {
    ma_sound sound;      ///< miniaudio sound object (shared source).
    bool     in_use;     ///< True if this slot is occupied.
    bool     valid;      ///< True if the sound was initialised successfully.
};

/** @brief A single player instance playing one of the loaded sounds. */
struct PlayerSlot {
    ma_sound sound;      ///< miniaudio sound object (this player's instance).
    bool     in_use;     ///< True if this slot is occupied.
    bool     valid;      ///< True if the sound was initialised successfully.
    f32      volume;     ///< This player's volume (0.0..1.0).
    f32      pitch;      ///< This player's pitch (0.5..2.0).
};

/** @brief The global miniaudio engine. */
static ma_engine   g_engine;
static bool        g_engine_ready = false;

/** @brief Fixed pool of loaded sounds. */
static SoundSlot   g_sounds[AUDIO_MAX_SOUNDS];

/** @brief Fixed pool of player instances. */
static PlayerSlot  g_players[AUDIO_MAX_PLAYERS];


/** @brief Initialise the audio engine. */
bool Audio::init()
{
    if (g_engine_ready) {
        return true;
    }

    for (u32 i = 0; i < AUDIO_MAX_SOUNDS; ++i) {
        g_sounds[i].in_use = false;
        g_sounds[i].valid = false;
    }
    for (u32 i = 0; i < AUDIO_MAX_PLAYERS; ++i) {
        g_players[i].in_use = false;
        g_players[i].valid = false;
    }

    ma_result result = ma_engine_init(NULL, &g_engine);
    if (result != MA_SUCCESS) {
        return false;
    }

    g_engine_ready = true;
    log_debug(LOG_SECTION_AUDIO, "engine initialised");
    return true;
}

/** @brief Shut down the audio engine. */
void Audio::shutdown()
{
    if (!g_engine_ready) {
        return;
    }

    log_debug(LOG_SECTION_AUDIO, "shutdown");
    for (u32 i = 0; i < AUDIO_MAX_PLAYERS; ++i) {
        if (g_players[i].in_use && g_players[i].valid) {
            ma_sound_uninit(&g_players[i].sound);
        }
        g_players[i].in_use = false;
        g_players[i].valid = false;
    }

    for (u32 i = 0; i < AUDIO_MAX_SOUNDS; ++i) {
        if (g_sounds[i].in_use && g_sounds[i].valid) {
            ma_sound_uninit(&g_sounds[i].sound);
        }
        g_sounds[i].in_use = false;
        g_sounds[i].valid = false;
    }

    ma_engine_uninit(&g_engine);
    g_engine_ready = false;
}

/** @brief Find a free slot in the sound pool, or -1. */
static i32 find_free_slot()
{
    for (u32 i = 0; i < AUDIO_MAX_SOUNDS; ++i) {
        if (!g_sounds[i].in_use) {
            return (i32)i;
        }
    }
    return -1;
}

/** @brief Load a sound from file into the pool. */
std::optional<SoundID> Audio::load(const char* path)
{
    if (!g_engine_ready) {
        return std::nullopt;
    }

    log_debug(LOG_SECTION_AUDIO, "loading '%s'", path);

    i32 slot = find_free_slot();
    if (slot < 0) {
        log_debug(LOG_SECTION_AUDIO, "sound pool full");
        return std::nullopt;
    }

    ma_result result = ma_sound_init_from_file(&g_engine, path, 0, NULL, NULL,
                                               &g_sounds[slot].sound);
    if (result != MA_SUCCESS) {
        log_debug(LOG_SECTION_AUDIO, "load failed '%s' (result=%d)", path, (int)result);
        return std::nullopt;
    }

    g_sounds[slot].in_use = true;
    g_sounds[slot].valid = true;
    return (SoundID)(slot + 1);
}

/** @brief Unload a sound and free its pool slot. */
void Audio::unload(SoundID sound)
{
    if (sound == 0 || sound > AUDIO_MAX_SOUNDS) {
        return;
    }

    u32 slot = (u32)(sound - 1);
    if (!g_sounds[slot].in_use) {
        return;
    }

    if (g_sounds[slot].valid) {
        ma_sound_uninit(&g_sounds[slot].sound);
    }
    g_sounds[slot].in_use = false;
    g_sounds[slot].valid = false;
}

/** @brief Resolve a handle to its sound slot index, or -1 if invalid. */
static i32 resolve_sound_slot(SoundID sound)
{
    if (sound == 0 || sound > AUDIO_MAX_SOUNDS) {
        return -1;
    }
    u32 slot = (u32)(sound - 1);
    if (!g_sounds[slot].in_use || !g_sounds[slot].valid) {
        return -1;
    }
    return (i32)slot;
}

/** @brief Resolve a handle to its player slot index, or -1 if invalid. */
static i32 resolve_player_slot(PlayerID player)
{
    if (player == 0 || player > AUDIO_MAX_PLAYERS) {
        return -1;
    }
    u32 slot = (u32)(player - 1);
    if (!g_players[slot].in_use || !g_players[slot].valid) {
        return -1;
    }
    return (i32)slot;
}

/** @brief Find a free slot in the player pool, or -1. */
static i32 find_free_player_slot()
{
    for (u32 i = 0; i < AUDIO_MAX_PLAYERS; ++i) {
        if (!g_players[i].in_use) {
            return (i32)i;
        }
    }
    return -1;
}

/** @brief Create a player instance for a sound. */
std::optional<PlayerID> Audio::make_player(PlayerConfig config)
{
    if (!g_engine_ready) {
        return std::nullopt;
    }

    log_debug(LOG_SECTION_AUDIO, "make_player sound=%u vol=%.2f pitch=%.2f loop=%d", config.sound, config.volume, config.pitch, config.loop);

    i32 sound_slot = resolve_sound_slot(config.sound);
    if (sound_slot < 0) {
        return std::nullopt;
    }

    i32 slot = find_free_player_slot();
    if (slot < 0) {
        log_debug(LOG_SECTION_AUDIO, "player pool full");
        return std::nullopt;
    }

    ma_result result = ma_sound_init_copy(
        &g_engine,
        &g_sounds[sound_slot].sound,
        0, NULL,
        &g_players[slot].sound);
    if (result != MA_SUCCESS) {
        return std::nullopt;
    }

    g_players[slot].in_use = true;
    g_players[slot].valid = true;
    g_players[slot].volume = config.volume;
    g_players[slot].pitch = config.pitch;

    ma_sound_set_volume(&g_players[slot].sound, config.volume);
    ma_sound_set_pitch(&g_players[slot].sound, config.pitch);
    ma_sound_set_looping(&g_players[slot].sound, config.loop ? MA_TRUE : MA_FALSE);

    if (config.start) {
        ma_sound_start(&g_players[slot].sound);
    }

    return (PlayerID)(slot + 1);
}

/** @brief Destroy a player instance. */
void Audio::destroy_player(PlayerID player)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return;
    }

    ma_sound_uninit(&g_players[slot].sound);
    g_players[slot].in_use = false;
    g_players[slot].valid = false;
    g_players[slot].volume = 1.0f;
    g_players[slot].pitch = 1.0f;
}

/** @brief Start playback of a player. */
void Audio::start(PlayerID player)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return;
    }
    ma_sound_start(&g_players[slot].sound);
}

/** @brief Stop playback without rewinding. */
void Audio::stop(PlayerID player)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return;
    }
    ma_sound_stop(&g_players[slot].sound);
}

/** @brief Rewind playback to the beginning. */
void Audio::seek_to_start(PlayerID player)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return;
    }
    ma_sound_seek_to_pcm_frame(&g_players[slot].sound, 0);
}

/** @brief Set a player's playback volume. */
void Audio::set_volume(PlayerID player, f32 volume)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return;
    }
    g_players[slot].volume = volume;
    ma_sound_set_volume(&g_players[slot].sound, volume);
}

/** @brief Set a player's pitch. */
void Audio::set_pitch(PlayerID player, f32 pitch)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return;
    }
    g_players[slot].pitch = pitch;
    ma_sound_set_pitch(&g_players[slot].sound, pitch);
}

/** @brief Set whether a player loops. */
void Audio::set_loop(PlayerID player, bool loop)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return;
    }
    ma_sound_set_looping(&g_players[slot].sound, loop ? MA_TRUE : MA_FALSE);
}

/** @brief True if the player is currently playing. */
bool Audio::is_playing(PlayerID player)
{
    i32 slot = resolve_player_slot(player);
    if (slot < 0) {
        return false;
    }
    return ma_sound_is_playing(&g_players[slot].sound) == MA_TRUE;
}

/** @brief Play a one-shot sound from file without preloading. */
void Audio::play_oneshot(const char* path, f32 volume)
{
    if (!g_engine_ready) {
        return;
    }
    ma_engine_play_sound(&g_engine, path, NULL);
    // ma_engine_play_sound does not expose the resulting sound for volume;
    // volume is applied on the next frames. Keep it simple: ignore per-call
    // volume here, callers wanting volume control should use load()/start().
    (void)volume;
}
