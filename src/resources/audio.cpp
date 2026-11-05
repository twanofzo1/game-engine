/**
 * @file audio.cpp
 * @brief Audio loading via dr_wav and dr_mp3 (WAV, MP3).
 */
#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#include "dr_wav.h"
#include "dr_mp3.h"

#include "resources/audio.hpp"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Audio {

/** @brief Case-insensitive check that `path` ends with `suffix`. */
static bool ends_with(const char* path, const char* suffix)
{
    usize path_len = strlen(path);
    usize suffix_len = strlen(suffix);
    if (suffix_len > path_len)
        return false;

    const char* start = path + (path_len - suffix_len);
    for (usize i = 0; i < suffix_len; ++i)
    {
        if (tolower((unsigned char)start[i]) != tolower((unsigned char)suffix[i]))
            return false;
    }
    return true;
}

/** @brief Copy decoded WAV data into `audio`. */
static void fill_from_wav(Audio* audio, drwav* wav)
{
    u64 frames = (u64)wav->totalPCMFrameCount;
    audio->sample_rate = (i32)wav->sampleRate;
    audio->channels = (i32)wav->channels;
    audio->sample_count = (usize)frames;
    audio->samples = (f32*)malloc(sizeof(f32) * (usize)frames * (usize)wav->channels);
    if (audio->samples)
        drwav_read_pcm_frames_f32(wav, frames, audio->samples);
}

/** @brief Copy decoded MP3 data into `audio`. */
static void fill_from_mp3(Audio* audio, drmp3* mp3)
{
    drmp3_uint64 frames = drmp3_get_pcm_frame_count(mp3);
    audio->sample_rate = (i32)mp3->sampleRate;
    audio->channels = (i32)mp3->channels;
    audio->sample_count = (usize)frames;
    audio->samples = (f32*)malloc(sizeof(f32) * (usize)frames * (usize)mp3->channels);
    if (audio->samples)
        drmp3_read_pcm_frames_f32(mp3, frames, audio->samples);
}

bool load_wav(Audio* audio, const u8* data, usize size)
{
    drwav wav;
    if (!drwav_init_memory(&wav, data, size, NULL))
        return false;

    fill_from_wav(audio, &wav);
    drwav_uninit(&wav);
    return audio->samples != NULL;
}

bool load_mp3(Audio* audio, const u8* data, usize size)
{
    drmp3 mp3;
    if (!drmp3_init_memory(&mp3, data, size, NULL))
        return false;

    fill_from_mp3(audio, &mp3);
    drmp3_uninit(&mp3);
    return audio->samples != NULL;
}

bool load(Audio* audio, const char* path)
{
    if (ends_with(path, ".mp3"))
    {
        drmp3 mp3;
        if (!drmp3_init_file(&mp3, path, NULL))
            return false;

        fill_from_mp3(audio, &mp3);
        drmp3_uninit(&mp3);
        return audio->samples != NULL;
    }

    drwav wav;
    if (!drwav_init_file(&wav, path, NULL))
        return false;

    fill_from_wav(audio, &wav);
    drwav_uninit(&wav);
    return audio->samples != NULL;
}

void destroy(Audio* audio)
{
    free(audio->samples);
    audio->samples = NULL;
    audio->sample_rate = 0;
    audio->channels = 0;
    audio->sample_count = 0;
}

} // namespace Audio
} // namespace Engine
