/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <system/debug.h>

#include "win32.h"

using namespace std::literals;
using namespace geo;

namespace {

    HANDLE hStdErr = nullptr;
    HANDLE hLoggerMutex = nullptr;
    bool logger_locked = false; // Secondary lock to prevent recursive log messages

    void write_console(std::wstring_view str)
    {
        DWORD chars_written = 0;

        while (!str.empty()) {
            if (!WriteConsoleW(hStdErr, str.data(), DWORD(str.length()), &chars_written, nullptr))
                break;
            else if (!chars_written)
                break;

            str = str.substr(chars_written);
        }
    }

} // namespace

void debug::init_logger()
{
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        hStdErr = GetStdHandle(STD_ERROR_HANDLE);
        SetConsoleMode(hStdErr, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
    }

    hLoggerMutex = CreateMutexW(nullptr, FALSE, nullptr);

    if (!hLoggerMutex) {
        DWORD errnum = GetLastError();
        win32::message_box(nullptr, (L"CreateMutexW failed: "s + win32::wcserror(errnum)).c_str(),
                           L"Error", MB_OK | MB_ICONERROR);
        detail::exit_fatal();
    }
}

void debug::shut_down_logger()
{
    CloseHandle(hLoggerMutex);
}

void debug::enable_console()
{
    if (hStdErr)
        return;

    if (AllocConsole()) {
        hStdErr = GetStdHandle(STD_ERROR_HANDLE);
        SetConsoleMode(hStdErr, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
    }
}

void debug::detail::vlog_src(const char* file, int line, LogLevel level, StringView fmt, FormatArgs args)
{
    std::wstring_view prefix;
    WORD prefix_attr;
    WORD msg_attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    if (!hStdErr && level != LogLevel::fatal)
        return;
    else if (level < LogLevel(1) || level > max_log_level)
        return;

    switch (level) {
#if LOG_LEVEL_MAX >= LOG_LEVEL_FATAL
        case LogLevel::fatal:
            prefix = L"FATAL ERROR: ";
            prefix_attr = FOREGROUND_RED | FOREGROUND_INTENSITY;
            msg_attr = FOREGROUND_RED;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_ERROR
        case LogLevel::error:
            prefix = L"ERROR: ";
            prefix_attr = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_WARNING
        case LogLevel::warning:
            prefix = L"WARNING: ";
            prefix_attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_INFO
        case LogLevel::info:
            prefix = L"INFO: ";
            prefix_attr = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_DEBUG
            prefix = L"DEBUG: ";
            prefix_attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_TRACE
            prefix = L"TRACE: ";
            prefix_attr = FOREGROUND_GREEN | FOREGROUND_BLUE;
            msg_attr = FOREGROUND_INTENSITY;
            break;
#endif
        default:
            return;
    }

    switch (WaitForSingleObject(hLoggerMutex, INFINITE)) {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            break;
        default: {
            UINT errnum = GetLastError();
            win32::message_box(nullptr, (L"WaitForSingleObject failed: "s + win32::wcserror(errnum)).c_str(),
                               L"Error", MB_OK | MB_ICONERROR);
            exit_fatal();
        }
    }

    if (logger_locked) {
        // Prevent recursive log messages, i.e., when a `formatter` attempts to log a message.
        return;
    } else {
        logger_locked = true;
    }

    auto msg = fmt::vformat(fmt, args);

    if (hStdErr) {
        SetConsoleTextAttribute(hStdErr, prefix_attr);
        write_console(prefix);
        SetConsoleTextAttribute(hStdErr, msg_attr);
    }

    if (file) {
        auto suffix = fmt::format(L" ({}:{})", Widen{file}, line);

        if (hStdErr) {
            SetConsoleTextAttribute(hStdErr, FOREGROUND_INTENSITY);
            write_console(suffix);
        }

        if (level == LogLevel::fatal)
            msg += suffix;
    }

    if (hStdErr) {
        SetConsoleTextAttribute(hStdErr, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        write_console(L"\r\n");
    }

    if (level == LogLevel::fatal) {
        win32::message_box(nullptr, msg.c_str(), L"Error", MB_OK | MB_ICONERROR);
        exit_fatal();
    }

    logger_locked = false;
    ReleaseMutex(hLoggerMutex);
}
