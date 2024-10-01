/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>

#include <geo/system/debug.h>
#include <geo/system/system.h>

using namespace geo;

// ANSI escape sequences for changing the color of log messages.
#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_FAINT "\x1b[2m"
#define ANSI_BLACK "\x1b[30m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_WHITE "\x1b[37m"

LogSystem::LogSystem()
{
    static char stderr_buffer[BUFSIZ];

    // stderr is unbuffered by default. Making it buffered can improve logging performance a bit by
    // reducing the number of syscalls made, likely to a single write() per message.
    ::setbuf(stderr, stderr_buffer);
}

LogSystem::~LogSystem()
{
}

LogSystem& LogSystem::get()
{
    static LogSystem instance;
    return instance;
}

bool LogSystem::log_begin(LogLevel level)
{
    const char* prefix;

    if (level < LogLevel::min || level > m_maxLevel)
        return false;

    switch (level) {
    case LogLevel::fatal:
        prefix = ANSI_BOLD ANSI_RED "FATAL ERROR: " ANSI_RESET;
        break;
    case LogLevel::error:
        prefix = ANSI_BOLD ANSI_RED "ERROR: " ANSI_RESET;
        break;
    case LogLevel::warning:
        prefix = ANSI_BOLD ANSI_YELLOW "WARNING: " ANSI_RESET;
        break;
    case LogLevel::notice:
        prefix = ANSI_BOLD ANSI_MAGENTA "NOTICE: " ANSI_RESET;
        break;
    case LogLevel::info:
        prefix = ANSI_BOLD ANSI_BLUE "INFO: " ANSI_RESET;
        break;
    case LogLevel::debug:
        prefix = ANSI_BOLD ANSI_GREEN "DEBUG: " ANSI_RESET;
        break;
    case LogLevel::trace:
        prefix = ANSI_FAINT ANSI_CYAN "TRACE: " ANSI_RESET ANSI_FAINT;
        break;
    default:
        return false;
    }

    m_mutex.lock();

    if (m_locked) {
        // Prevent recursive log messages on the same thread.
        m_mutex.unlock();
        return false;
    }

    m_locked = true;
    ::fputs(prefix, stderr);
    return true;
}

void LogSystem::log_write(oschar_t ch)
{
    ::fputc(ch, stderr);
}

void LogSystem::log_end(const oschar_t* file, int line)
{
    if (file) {
        ::fprintf(stderr, ANSI_FAINT " (%s:%d)", file, line);
    }

    ::fputs(ANSI_RESET "\n", stderr);
    ::fflush(stderr);
    m_locked = false;
    m_mutex.unlock();
}

void LogSystem::fatal_begin()
{
    m_mutex.lock();

    if (m_locked) {
        // We're interrupting another log message.
        ::fputc('\n', stderr);
        m_fatalMessage.clear();
    } else {
        m_locked = true;
    }
}

void LogSystem::fatal_write(oschar_t ch)
{
    m_fatalMessage.push_back(ch);
}

void LogSystem::fatal_end(const oschar_t* file, int line)
{
    ::fputs(ANSI_BOLD ANSI_RED "FATAL ERROR: " ANSI_RESET, stderr);
    ::fputs(m_fatalMessage.c_str(), stderr);

    if (file) {
        ::fprintf(stderr, ANSI_FAINT " (%s:%d)", file, line);
        format::write([this](oschar_t ch) { m_fatalMessage.push_back(ch); }, " ({}:{})", file, line);
    }

    ::fputs(ANSI_RESET "\n", stderr);
    ::fflush(stderr);
    system::show_error_dialog(m_fatalMessage.c_str());
    ::exit(EXIT_FAILURE);
}
