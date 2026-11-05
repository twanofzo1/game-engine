/**
 * @file manager.hpp
 * @brief Declares the audio playback manager.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace AudioManager {

/** Audio playback manager (owns a miniaudio engine). */
struct Manager {
    void* engine; ///< Opaque miniaudio engine handle.
};

/** @brief Initialize the audio device and engine. */
bool init(Manager* manager);

/** @brief Shut down the audio engine and free resources. */
void shutdown(Manager* manager);

/** @brief Play an audio file (WAV, MP3, FLAC, ...). */
bool play_file(Manager* manager, const char* path);

/** @brief Set the master volume (0..1). */
void set_volume(Manager* manager, f32 volume);

/** @brief Stop all playing sounds. */
void stop_all(Manager* manager);

} // namespace AudioManager
} // namespace Engine
