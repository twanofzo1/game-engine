/**
 * @file log.cpp
 * @brief Logging system implementation.
 */
#include "common/log/log.hpp"

#include <stdio.h>

/** @brief Global minimum level; messages below this are dropped. */
static LogLevel g_global_level = LOG_LEVEL_INFO;

/** @brief Per-section enabled flags. */
static bool g_section_enabled[LOG_SECTION_COUNT];

/** @brief Custom output callback (NULL = use the terminal). */
static LogOutput g_output = nullptr;

/** @brief Opaque user data passed to the output callback. */
static void* g_output_user_data = nullptr;

/** @brief Human-readable names for each section. */
static const char* section_name(LogSection section)
{
    switch (section)
    {
        case LOG_SECTION_CORE:      return "core";
        case LOG_SECTION_WINDOW:    return "window";
        case LOG_SECTION_RENDERER:  return "renderer";
        case LOG_SECTION_SHADER:    return "shader";
        case LOG_SECTION_TEXTURE:   return "texture";
        case LOG_SECTION_AUDIO:     return "audio";
        case LOG_SECTION_SERIALISE: return "serialise";
        case LOG_SECTION_EDITOR:    return "editor";
        case LOG_SECTION_RUNTIME:   return "runtime";
        default:                    return "unknown";
    }
}

/** @brief Human-readable label for a level, for the output prefix. */
static const char* level_label(LogLevel level)
{
    switch (level)
    {
        case LOG_LEVEL_DEBUG:   return "DEBUG";
        case LOG_LEVEL_INFO:    return "INFO";
        case LOG_LEVEL_WARNING: return "WARN";
        case LOG_LEVEL_ERROR:   return "ERROR";
        default:                return "?";
    }
}

/** @brief Initialise the logger: all sections enabled, info threshold. */
void Log::init()
{
    g_global_level = LOG_LEVEL_INFO;
    g_output = nullptr;
    g_output_user_data = nullptr;
    for (u32 i = 0; i < LOG_SECTION_COUNT; ++i) {
        g_section_enabled[i] = true;
    }
}

/** @brief Set the global minimum level. */
void Log::set_global_level(LogLevel level)
{
    g_global_level = level;
}

/** @brief Enable or disable a section. */
void Log::set_section_enabled(LogSection section, bool enabled)
{
    if (section < 0 || section >= LOG_SECTION_COUNT) {
        return;
    }
    g_section_enabled[section] = enabled;
}

/** @brief Set a custom output sink, or NULL to restore the terminal. */
void Log::set_output(LogOutput output, void* user_data)
{
    g_output = output;
    g_output_user_data = user_data;
}

/** @brief Emit a message using a pre-built va_list. */
void Log::log_va(LogSection section, LogLevel level, const char* fmt, va_list args)
{
    if (section < 0 || section >= LOG_SECTION_COUNT) {
        return;
    }
    if (!g_section_enabled[section]) {
        return;
    }
    if (level < g_global_level) {
        return;
    }

    // Build the full formatted line (prefix + message + newline).
    char prefix[64];
    snprintf(prefix, sizeof(prefix), "[%s] %s: ", level_label(level), section_name(section));

    char line[1024];
    i32 written = snprintf(line, sizeof(line), "%s", prefix);
    if (written < 0 || (usize)written >= sizeof(line)) {
        return;
    }
    vsnprintf(line + written, sizeof(line) - (usize)written, fmt, args);

    if (g_output != nullptr) {
        g_output(line, g_output_user_data);
        return;
    }

    FILE* out = (level >= LOG_LEVEL_WARNING) ? stderr : stdout;
    fprintf(out, "%s\n", line);
}

/** @brief Emit a formatted message for a section at a level. */
void Log::log(LogSection section, LogLevel level, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log::log_va(section, level, fmt, args);
    va_end(args);
}
