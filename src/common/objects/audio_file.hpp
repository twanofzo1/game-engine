/**
 * @file audio_file.hpp
 * @brief Audio file type.
 */
#pragma once
#include "common/core/types.hpp"

using AudioID = u32;

/** @brief An audio file resource. */
struct AudioFile {
    AudioID id; ///< Unique identifier.
    std::string name; ///< Name of the audio file.
    std::string path; ///< Path to the audio file.
};