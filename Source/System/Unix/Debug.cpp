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

#include <Math/Math.h>
#include <System/Debug.h>

using namespace Geo;

namespace {

    struct LoggerState {
        std::recursive_mutex mutex;
        bool locked = false; // Secondary lock to prevent recursive log messages
        std::string fatalMsg;

        static LoggerState& GetInstance()
        {
            static LoggerState instance;
            return instance;
        }

    private:
        LoggerState()
        {
            static char stderrBuffer[BUFSIZ];

            // stderr is unbuffered by default. By making it line-buffered, we can improve logging
            // performance by reducing the number of syscalls, typically to one write() per message.
            setvbuf(stderr, stderrBuffer, _IOLBF, sizeof(stderrBuffer));
        }

        LoggerState(const LoggerState&) = delete;
        ~LoggerState() = default;

        LoggerState& operator=(const LoggerState&) = delete;
    };

    LogLevel s_maxLogLevel = LogLevel(LOG_LEVEL_DEFAULT);

} // namespace

void Debug::EnableConsole()
{
}

void Debug::SetMaxLogLevel(LogLevel level)
{
    s_maxLogLevel = Math::Clamp(level, LogLevel::Off, LogLevel(LOG_LEVEL_MAX));
}

bool Debug::Internal::BeginLogMessage(LogLevel level)
{
    const char* prefix;

    if (level > s_maxLogLevel)
        return false;

    // Note: These prefixes contain ANSI escape sequences to change the color and appearance of
    // output text.
    switch (level) {
#if LOG_LEVEL_MAX >= LOG_LEVEL_ERROR
        case LogLevel::Error:
            prefix = "\033[1;31mERROR: \033[0m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_WARNING
        case LogLevel::Warning:
            prefix = "\033[1;33mWARNING: \033[0m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_INFO
        case LogLevel::Info:
            prefix = "\033[1;34mINFO: \033[0m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_DEBUG
        case LogLevel::Debug:
            prefix = "\033[1;32mDEBUG: \033[0;32m";
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_TRACE
        case LogLevel::Trace:
            prefix = "\033[2;36mTRACE: \033[0;2m";
            break;
#endif
        default:
            return false;
    }

    auto& logger = LoggerState::GetInstance();
    logger.mutex.lock();

    if (logger.locked) {
        // We're already in the middle of logging another message.
        // Stop now to prevent recursive logging.
        logger.mutex.unlock();
        return false;
    }

    logger.locked = true;
    fputs(prefix, stderr);
    return true;
}

void Debug::Internal::WriteLogMessage(OsStringView str)
{
    fwrite(str.data(), 1, str.length(), stderr);
}

void Debug::Internal::EndLogMessage()
{
    EndLogMessage(nullptr, 0);
}

void Debug::Internal::EndLogMessage(const oschar_t* file, int line)
{
    auto& logger = LoggerState::GetInstance();

    if (file)
        fprintf(stderr, " \033[0;2m(%s:%d)", file, line);

    fputs("\033[0m\n", stderr);
    logger.locked = false;
    logger.mutex.unlock();
}

void Debug::Internal::BeginFatalError()
{
    auto& logger = LoggerState::GetInstance();
    logger.mutex.lock();

    if (logger.locked) {
        // We're interrupting another log message.
        fputc('\n', stderr);
        logger.fatalMsg.clear();
    } else {
        logger.locked = true;
    }
}

void Debug::Internal::WriteFatalError(OsStringView str)
{
    LoggerState::GetInstance().fatalMsg += str;
}

void Debug::Internal::EndFatalError()
{
    EndFatalError(nullptr, 0);
}

void Debug::Internal::EndFatalError(const oschar_t* file, int line)
{
    auto& logger = LoggerState::GetInstance();

    fputs("\033[1;31mFATAL ERROR: \033[0;31m", stderr);
    fputs(logger.fatalMsg.c_str(), stderr);

    if (file)
        fprintf(stderr, " \033[2m(%s:%d)", file, line);

    fputs("\033[0m\n", stderr);
    ExitFatal();
}

void Debug::Internal::ExitFatal()
{
    exit(EXIT_FAILURE);
}
