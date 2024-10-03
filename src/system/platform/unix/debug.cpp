/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>

#include <mutex>

#include <geo/system/debug.h>
#include <geo/system/system.h>

using namespace geo;

namespace geo::debug::internal {

    struct LoggerState {
        std::recursive_mutex mutex;
        bool locked = false; // Secondary lock to prevent recursive messages
        std::string fatalMessage;

        static LoggerState& get_instance()
        {
            static LoggerState instance;
            return instance;
        }

    private:
        LoggerState()
        {
            // stderr is unbuffered by default. By making it buffered, we can improve logging
            // performance a bit by reducing the number of syscalls made, likely to a single write()
            // per message.
            static char stderrBuffer[BUFSIZ];
            ::setbuf(stderr, stderrBuffer);
        }

        ~LoggerState()
        {
        }
    };

} // namespace geo::debug::internal

namespace {

    LogLevel s_maxLogLevel =
#ifdef NDEBUG
        LogLevel::notice;
#else
        LogLevel::debug;
#endif

} // namespace

void debug::enable_verbose()
{
#ifdef NDEBUG
    s_maxLogLevel = LogLevel::info;
#else
    s_maxLogLevel = LogLevel::trace;
#endif
}

bool debug::internal::log_begin(LogLevel level, out<LoggerState*> outState)
{
    static const char* prefixes[] = {
        // NOTE: The weird sequences here are ANSI escape sequences for changing text colors.
        "\x1b[1;31mERROR: \x1b[0m",
        "\x1b[1;33mWARNING: \x1b[0m",
        "\x1b[1;35mNOTICE: \x1b[0m",
        "\x1b[1;34mINFO: \x1b[0m",
        "\x1b[1;32mDEBUG: \x1b[0m",
        "\x1b[2;36mTRACE: \x1b[0;2m",
    };

    if (level > s_maxLogLevel)
        return false;

    auto state = &LoggerState::get_instance();
    state->mutex.lock();

    if (state->locked) {
        // Don't start a log message when we're already in the middle of one on the same thread.
        state->mutex.unlock();
        return false;
    }

    state->locked = true;
    ::fputs(prefixes[unsigned(level)], stderr);
    *outState = state;
    return true;
}

void debug::internal::log_write(oschar_t ch)
{
    ::fputc(ch, stderr);
}

void debug::internal::log_end(LoggerState* state)
{
    ::fputs("\x1b[0m\n", stderr);
    ::fflush(stderr);
    state->locked = false;
    state->mutex.unlock();
}

void debug::internal::log_end(LoggerState* state, const oschar_t* sourceFileName, int sourceLine)
{
    ::fprintf(stderr, " \x1b[2m(%s:%d)", sourceFileName, sourceLine);
    internal::log_end(state);
}

void debug::internal::fatal_begin(out<LoggerState*> outState)
{
    auto state = &LoggerState::get_instance();
    state->mutex.lock();

    if (state->locked) {
        // We're interrupting another message.
        ::fputc('\n', stderr);
        state->fatalMessage.clear();
    } else {
        state->locked = true;
    }

    *outState = state;
}

void debug::internal::fatal_write(LoggerState* state, oschar_t ch)
{
    state->fatalMessage.push_back(ch);
}

void debug::internal::fatal_end(LoggerState* state, const oschar_t* sourceFileName, int sourceLine)
{
    ::fputs("\x1b[1;31mFATAL ERROR: \x1b[0m", stderr);
    ::fputs(state->fatalMessage.c_str(), stderr);

    if (sourceFileName) {
        ::fprintf(stderr, " \x1b[2m(%s:%d)", sourceFileName, sourceLine);
        format::write([&](oschar_t ch) { state->fatalMessage.push_back(ch); }, " ({}:{})", sourceFileName, sourceLine);
    }

    ::fputs("\x1b[0m\n", stderr);
    ::fflush(stderr);
    system::show_error_dialog(state->fatalMessage.c_str());
    ::exit(EXIT_FAILURE);
}
