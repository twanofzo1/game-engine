/**
 * @file test_audio.cpp
 * @brief Tests for audio decoding (dr_wav backend).
 *
 * Builds a valid 16-bit mono WAV in memory so the test is self-contained:
 * it verifies header parsing, sample conversion and the failure paths for
 * garbage input.
 */
#include "framework.hpp"

#include "resources/audio.hpp"

#include <stdio.h>
#include <string.h>

/** @brief Write a little-endian u16 at `p`. */
static void put_u16(u8* p, u16 v)
{
    p[0] = (u8)(v & 0xFFu);
    p[1] = (u8)((v >> 8) & 0xFFu);
}

/** @brief Write a little-endian u32 at `p`. */
static void put_u32(u8* p, u32 v)
{
    p[0] = (u8)(v & 0xFFu);
    p[1] = (u8)((v >> 8) & 0xFFu);
    p[2] = (u8)((v >> 16) & 0xFFu);
    p[3] = (u8)((v >> 24) & 0xFFu);
}

/**
 * @brief Build a 16-bit mono PCM WAV in memory.
 * @return Size of the WAV in bytes.
 */
static usize make_wav_mono16(u8* out, i32 sample_rate, const i16* samples, u32 count)
{
    u32 data_size = count * 2u;
    u32 byte_rate = (u32)sample_rate * 2u; /* mono, 2 bytes per sample */

    memcpy(out, "RIFF", 4);
    put_u32(out + 4, 36u + data_size);
    memcpy(out + 8, "WAVE", 4);
    memcpy(out + 12, "fmt ", 4);
    put_u32(out + 16, 16);              /* fmt chunk size */
    put_u16(out + 20, 1);               /* PCM */
    put_u16(out + 22, 1);               /* mono */
    put_u32(out + 24, (u32)sample_rate);
    put_u32(out + 28, byte_rate);
    put_u16(out + 32, 2);               /* block align */
    put_u16(out + 34, 16);              /* bits per sample */
    memcpy(out + 36, "data", 4);
    put_u32(out + 40, data_size);

    for (u32 i = 0; i < count; ++i)
        put_u16(out + 44 + i * 2u, (u16)samples[i]);

    return 44 + data_size;
}

void test_audio(void)
{
    i16 samples[4] = { 0, 16384, -16384, 32767 };
    u8 wav[1024];
    usize wav_size = make_wav_mono16(wav, 44100, samples, 4);

    Engine::Audio::Audio audio;
    TEST_CHECK(Engine::Audio::load_wav(&audio, wav, wav_size));
    TEST_CHECK(audio.sample_rate == 44100);
    TEST_CHECK(audio.channels == 1);
    TEST_CHECK(audio.sample_count == 4);
    TEST_CHECK(audio.samples != NULL);

    if (audio.samples)
    {
        /* 16-bit PCM converts to float as sample / 32768. */
        TEST_CHECK_FEQ(audio.samples[0], 0.0f, 1e-6);
        TEST_CHECK_FEQ(audio.samples[1], 0.5f, 1e-6);
        TEST_CHECK_FEQ(audio.samples[2], -0.5f, 1e-6);
        TEST_CHECK_FEQ(audio.samples[3], 32767.0f / 32768.0f, 1e-6);
    }

    Engine::Audio::destroy(&audio);
    TEST_CHECK(audio.samples == NULL);

    /* Garbage input fails cleanly for both decoders. */
    TEST_CHECK(!Engine::Audio::load_wav(&audio, (const u8*)"garbage", 7));
    TEST_CHECK(!Engine::Audio::load_mp3(&audio, (const u8*)"garbage", 7));

    /* File-based load uses the extension to pick the decoder. */
    FILE* f = fopen("__audio_test__.wav", "wb");
    TEST_CHECK(f != NULL);
    if (f)
    {
        fwrite(wav, 1, wav_size, f);
        fclose(f);

        Engine::Audio::Audio from_file;
        TEST_CHECK(Engine::Audio::load(&from_file, "__audio_test__.wav"));
        TEST_CHECK(from_file.sample_count == 4);
        TEST_CHECK(from_file.sample_rate == 44100);
        Engine::Audio::destroy(&from_file);
        remove("__audio_test__.wav");
    }

    /* A .mp3 file that is not MP3 data is rejected. */
    f = fopen("__audio_test__.mp3", "wb");
    TEST_CHECK(f != NULL);
    if (f)
    {
        fwrite("garbage", 1, 7, f);
        fclose(f);

        Engine::Audio::Audio bad;
        TEST_CHECK(!Engine::Audio::load(&bad, "__audio_test__.mp3"));
        remove("__audio_test__.mp3");
    }
}
