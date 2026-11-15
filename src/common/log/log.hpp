/**
 * @file log.hpp
 * @brief Sectioned, levelled logging with compile-time debug-only support.
 *
 * Log messages are tagged with a section (e.g. audio, renderer, window) and a
 * level (debug/info/warning/error). Sections can be enabled/disabled and given
 * their own minimum level, so noisy systems can be filtered independently.
 * Debug messages are compiled out entirely in release builds (NDEBUG).
 */
#pragma once

#include "common/core/typedefs.hpp"

#include <stdarg.h>

/** @brief Severity of a log message, ordered least to most severe. */
enum LogLevel {
    LOG_LEVEL_DEBUG   = 0, ///< Verbose diagnostics (compiled out in release).
    LOG_LEVEL_INFO    = 1, ///< Normal informational messages.
    LOG_LEVEL_WARNING = 2, ///< Recoverable problems.
    LOG_LEVEL_ERROR   = 3, ///< Unrecoverable problems.
};

/** @brief A logging section/category (e.g. audio, renderer, window). */
enum LogSection {
    LOG_SECTION_CORE       = 0, ///< Foundation (types, memory, io).
    LOG_SECTION_WINDOW     = 1, ///< Window creation and GLFW.
    LOG_SECTION_RENDERER   = 2, ///< Batched rendering.
    LOG_SECTION_SHADER     = 3, ///< Shader compile/link.
    LOG_SECTION_TEXTURE    = 4, ///< Texture loading.
    LOG_SECTION_AUDIO      = 5, ///< Sound playback.
    LOG_SECTION_SERIALISE  = 6, ///< YAML (de)serialisation.
    LOG_SECTION_EDITOR     = 7, ///< Editor application.
    LOG_SECTION_RUNTIME    = 8, ///< Runtime application.
    LOG_SECTION_COUNT      = 9, ///< Number of sections (not a real section).
};

/**
 * @brief Callback that receives a fully formatted log line.
 *
 * @param line The formatted message (with prefix + newline) to output.
 * @param user_data Opaque context supplied when the output was set
 *                  (e.g. a FILE* to write to).
 */
typedef void (*LogOutput)(const char* line, void* user_data);

/**
 * @brief Global logging system.
 *
 * Set the desired minimum level and per-section toggles once at startup, then
 * call Log::log() (or the convenience wrappers) throughout the code. Output
 * goes to the terminal by default; call set_output() to redirect it (e.g. to
 * a file).
 */
struct Log {
    /** @brief Initialise the logger to sensible defaults (all sections on). */
    static void init();

    /** @brief Set the global minimum level (below this nothing is logged). */
    static void set_global_level(LogLevel level);

    /** @brief Enable or disable a section. */
    static void set_section_enabled(LogSection section, bool enabled);

    /**
     * @brief Set a custom output sink, or NULL to restore the terminal.
     *
     * @param output Callback called with each formatted line. NULL resets to
     *               the default terminal output.
     * @param user_data Opaque pointer passed back to `output` (e.g. FILE*).
     */
    static void set_output(LogOutput output, void* user_data);

    /** @brief Emit a formatted message for a section at a level. */
    static void log(LogSection section, LogLevel level, const char* fmt, ...);

    /** @brief Emit a message using a pre-built va_list. */
    static void log_va(LogSection section, LogLevel level, const char* fmt, va_list args);
};

/** @brief Convenience wrapper for DEBUG-level messages. */
static inline void log_debug(LogSection section, const char* fmt, ...)
{
#ifndef NDEBUG
    va_list args;
    va_start(args, fmt);
    Log::log_va(section, LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
#else
    (void)section; (void)fmt;
#endif
}

/** @brief Convenience wrapper for INFO-level messages. */
static inline void log_info(LogSection section, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log::log_va(section, LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

/** @brief Convenience wrapper for WARNING-level messages. */
static inline void log_warning(LogSection section, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log::log_va(section, LOG_LEVEL_WARNING, fmt, args);
    va_end(args);
}

/** @brief Convenience wrapper for ERROR-level messages. */
static inline void log_error(LogSection section, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log::log_va(section, LOG_LEVEL_ERROR, fmt, args);
    va_end(args);
}
