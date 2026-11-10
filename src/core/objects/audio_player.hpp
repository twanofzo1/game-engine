/**
 * @file audio_player.hpp
 * @brief Audio player type.
 */
#pragma once
#include "core/types.hpp"
#include "core/objects/audio_file.hpp"

/** @brief Plays back audio files. */
struct AudioPlayer {
    std::vector<AudioID> playlist; ///< List of names and audio file references to play.
    f32 volume; ///< Playback volume (0.0 to 1.0).
    bool loop; ///< Whether to loop the playlist.
};