/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <err.h>
#include <pthread.h>
#include <stdio.h>

#include <system/debug.h>

using namespace geo;

namespace {

    // Output iterator that calls `fputc(ch, stderr)` each time it's assigned a char.
    class LogIterator {
    public:
        using difference_type = ptrdiff_t;
        using iterator_category = std::output_iterator_tag;
        using pointer = char*;
        using reference = char&;
        using value_type = char;

        constexpr LogIterator() = default;
        constexpr LogIterator(const LogIterator& other) = default;
        constexpr LogIterator& operator=(const LogIterator& other) = default;

        LogIterator& operator=(char ch)
        {
            fputc(ch, stderr);
            return *this;
        }

        constexpr LogIterator& operator*() { return *this; }
        constexpr LogIterator& operator++() { return *this; }
        constexpr LogIterator& operator++(int) { return *this; }
    };

    pthread_mutex_t logger_mutex = PTHREAD_MUTEX_INITIALIZER;
    bool logger_locked = false; // Secondary lock to prevent recursive logging.

} // namespace

void debug::init_logger()
{
    static char stderr_buffer[BUFSIZ];

    if (pthread_mutex_init(&logger_mutex, nullptr))
        errx(EXIT_FAILURE, "Failed to initialize logger mutex");

    // stderr is unbuffered by default. By making it buffered, we improve logging performance by
    // reducing the number of syscalls, typically to one write() per message.
    setvbuf(stderr, stderr_buffer, _IOLBF, sizeof(stderr_buffer));
}

void debug::shut_down_logger()
{
    pthread_mutex_destroy(&logger_mutex);
}

void debug::detail::vlog_src(const char* file, int line, LogLevel level, StringView fmt, FormatArgs args)
{
    const char* prefix;

    if (level < LogLevel(1) || level > max_log_level)
        return;

    // Note: These contain ANSI escape sequences that change the color and format of output text.
    switch (level) {
#if LOG_LEVEL_MAX >= LOG_LEVEL_FATAL
        case LogLevel::fatal:
            prefix = "\033[1;31mFATAL ERROR: \033[0;31m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_ERROR
        case LogLevel::error:
            prefix = "\033[1;31mERROR: \033[0m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_WARNING
        case LogLevel::warning:
            prefix = "\033[1;33mWARNING: \033[0m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_INFO
        case LogLevel::info:
            prefix = "\033[1;34mINFO: \033[0m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_DEBUG
        case LogLevel::debug:
            prefix = "\033[1;32mDEBUG: \033[0;32m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_TRACE
        case LogLevel::trace:
            prefix = "\033[2;36mTRACE: \033[0;2m";
            break;
#endif
        default:
            return;
    }

    if (pthread_mutex_lock(&logger_mutex))
        return;

    // Prevent recursive log messages (i.e. if a formatter attempts to log a message).
    if (logger_locked)
        return;

    logger_locked = true;
    fputs(prefix, stderr);
    fmt::vformat_to(LogIterator{}, fmt, args);

    if (file)
        fprintf(stderr, " \033[2m(%s:%d)", file, line);

    fputs("\033[0m\n", stderr);

    if (level == LogLevel::fatal)
        exit_fatal();

    logger_locked = false;
    pthread_mutex_unlock(&logger_mutex);
}
