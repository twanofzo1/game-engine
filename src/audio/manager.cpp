/**
 * @file manager.cpp
 * @brief Audio playback manager implementation (miniaudio backend).
 */
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "audio/manager.hpp"

#include <stdlib.h>

namespace Engine {
namespace AudioManager {

bool init(Manager* manager)
{
    manager->engine = malloc(sizeof(ma_engine));
    if (!manager->engine)
        return false;

    if (ma_engine_init(NULL, (ma_engine*)manager->engine) != MA_SUCCESS)
    {
        free(manager->engine);
        manager->engine = NULL;
        return false;
    }
    return true;
}

void shutdown(Manager* manager)
{
    if (!manager->engine)
        return;

    ma_engine_uninit((ma_engine*)manager->engine);
    free(manager->engine);
    manager->engine = NULL;
}

bool play_file(Manager* manager, const char* path)
{
    if (!manager->engine)
        return false;
    return ma_engine_play_sound((ma_engine*)manager->engine, path, NULL) == MA_SUCCESS;
}

void set_volume(Manager* manager, f32 volume)
{
    if (manager->engine)
        ma_engine_set_volume((ma_engine*)manager->engine, volume);
}

void stop_all(Manager* manager)
{
    if (manager->engine)
        ma_engine_stop((ma_engine*)manager->engine);
}

} // namespace AudioManager
} // namespace Engine
