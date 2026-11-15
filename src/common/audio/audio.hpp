/**
 * @file audio.hpp
 * @brief Sound handling system on top of miniaudio.
 *
 * Wraps a single miniaudio engine (ma_engine) behind a struct of static
 * functions. Sounds are loaded from file into a fixed pool (SoundID), and
 * each sound is played through one or more player instances (PlayerID).
 * Every player has its own live volume and pitch, so multiple players can
 * share a sound while sounding differently. Falls back gracefully if audio
 * initialisation fails (all playback calls become no-ops).
 */
#pragma once

#include <optional>

#include "common/core/typedefs.hpp"

/** Maximum number of distinct sounds that can be loaded at once. */
#define AUDIO_MAX_SOUNDS 64

/** Maximum number of player instances that can exist at once. */
#define AUDIO_MAX_PLAYERS 128

/** Handle to a loaded sound (0 is invalid). */
using SoundID = u32;

/** Handle to a player instance (0 is invalid). */
using PlayerID = u32;

/**
 * @brief Config used to create a player instance.
 *
 * Defaults let a caller create a player with sane values and tweak only what
 * they care about.
 */
struct PlayerConfig {
    SoundID sound = 0;      ///< Sound to play (required).
    f32 volume = 1.0f;      ///< Playback volume (0.0..1.0).
    f32 pitch = 1.0f;       ///< Playback pitch (0.5..2.0; 1.0 = normal).
    bool loop = false;      ///< Loop the sound once it ends.
    bool start = true;      ///< Begin playback immediately.
};

/**
 * @brief Global sound system.
 *
 * Call init() once at startup and shutdown() at exit. Sounds are preloaded
 * with load() (returning a SoundID); a player instance is spawned from a
 * sound with make_player() (returning a PlayerID). Each player keeps its own
 * volume/pitch and can be controlled independently.
 */
struct Audio {
    /** @brief Initialise the audio engine, or nullopt on failure. */
    static bool init();

    /** @brief Shut down and release the audio engine. */
    static void shutdown();

    /** @brief Load a sound from file, or nullopt on failure. */
    static std::optional<SoundID> load(const char* path);

    /** @brief Unload a sound and free its pool slot. */
    static void unload(SoundID sound);

    /** @brief Create a player instance for a sound, or nullopt on failure. */
    static std::optional<PlayerID> make_player(PlayerConfig config);

    /** @brief Destroy a player instance. */
    static void destroy_player(PlayerID player);

    /** @brief Start (or restart) playback of a player. */
    static void start(PlayerID player);

    /** @brief Stop playback without rewinding. */
    static void stop(PlayerID player);

    /** @brief Rewind playback to the beginning. */
    static void seek_to_start(PlayerID player);

    /** @brief Set a player's playback volume (0.0..1.0). */
    static void set_volume(PlayerID player, f32 volume);

    /** @brief Set a player's pitch (0.5..2.0; 1.0 = normal). */
    static void set_pitch(PlayerID player, f32 pitch);

    /** @brief Set whether a player loops its sound. */
    static void set_loop(PlayerID player, bool loop);

    /** @brief True if the player is currently playing. */
    static bool is_playing(PlayerID player);

    /** @brief Play a one-shot sound from file without preloading. */
    static void play_oneshot(const char* path, f32 volume);
};
