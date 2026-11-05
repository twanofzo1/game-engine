/**
 * @file audio.hpp
 * @brief Declares a decoded PCM audio resource (WAV and MP3 backends).
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Audio {

/** Decoded PCM audio as normalized float samples. */
struct Audio {
    i32   sample_rate;  ///< Samples per second.
    i32   channels;     ///< Number of channels.
    usize sample_count; ///< Frames (samples per channel).
    f32*  samples;      ///< Interleaved samples in [-1, 1].
};

/** @brief Free the sample buffer owned by `audio`. */
void destroy(Audio* audio);

/**
 * @brief Decode a WAV from memory.
 * @return True on success.
 */
bool load_wav(Audio* audio, const u8* data, usize size);

/**
 * @brief Decode an MP3 from memory.
 * @return True on success.
 */
bool load_mp3(Audio* audio, const u8* data, usize size);

/**
 * @brief Load and decode an audio file (.wav/.mp3, detected by extension).
 * @return True on success.
 */
bool load(Audio* audio, const char* path);

} // namespace Audio
} // namespace Engine
