/**
 * @file test_image.cpp
 * @brief Tests for image decoding (stb_image backend).
 *
 * Builds a tiny uncompressed 24-bit BMP in memory so the test is
 * self-contained: it verifies decode success, dimensions, RGBA conversion
 * and the failure path for garbage input.
 */
#include "framework.hpp"

#include "resources/image.hpp"

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
 * @brief Build a 2x2 top-down 24-bit BMP: red/green on top, blue/white below.
 * @param out  Destination buffer (>= 70 bytes).
 * @return Size of the BMP in bytes.
 */
static usize make_bmp(u8* out)
{
    u8* px;

    memset(out, 0, 70);
    out[0] = 'B';
    out[1] = 'M';
    put_u32(out + 2, 70);                 /* file size */
    put_u32(out + 10, 54);                /* pixel data offset */
    put_u32(out + 14, 40);                /* DIB header size */
    put_u32(out + 18, 2);                 /* width */
    put_u32(out + 22, 0xFFFFFFFEu);       /* height = -2 (top-down) */
    put_u16(out + 26, 1);                 /* planes */
    put_u16(out + 28, 24);                /* 24 bits per pixel */
    put_u32(out + 30, 0);                 /* BI_RGB (no compression) */
    put_u32(out + 34, 16);                /* image size */

    px = out + 54;
    /* Row 0 (top): red, green, then 2 padding bytes. */
    px[0] = 0;   px[1] = 0;   px[2] = 255; /* red */
    px[3] = 0;   px[4] = 255; px[5] = 0;   /* green */
    px[6] = 0;   px[7] = 0;
    /* Row 1: blue, white, then 2 padding bytes. */
    px[8]  = 255; px[9]  = 0;   px[10] = 0;   /* blue */
    px[11] = 255; px[12] = 255; px[13] = 255; /* white */
    px[14] = 0;   px[15] = 0;

    return 70;
}

void test_image(void)
{
    u8 bmp[70];
    usize bmp_size = make_bmp(bmp);

    Engine::Image::Image img;
    TEST_CHECK(Engine::Image::load_memory(&img, bmp, bmp_size));
    TEST_CHECK(img.width == 2 && img.height == 2);
    TEST_CHECK(img.channels == 4);
    TEST_CHECK(img.pixels != NULL);

    if (img.pixels)
    {
        /* Top-left is red, bottom-right is white (RGBA). */
        u8* tl = img.pixels;
        u8* br = img.pixels + (1 * 2 + 1) * 4;
        TEST_CHECK(tl[0] == 255 && tl[1] == 0 && tl[2] == 0 && tl[3] == 255);
        TEST_CHECK(br[0] == 255 && br[1] == 255 && br[2] == 255 && br[3] == 255);
    }

    Engine::Image::destroy(&img);
    TEST_CHECK(img.pixels == NULL);
    TEST_CHECK(img.width == 0 && img.height == 0);

    /* Garbage input fails cleanly. */
    TEST_CHECK(!Engine::Image::load_memory(&img, (const u8*)"not an image", 12));
}
