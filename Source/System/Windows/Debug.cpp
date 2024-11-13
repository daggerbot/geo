/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>
#include <windows.h>

#include <Core/GameDefs.h>
#include <Math/Math.h>
#include <System/Debug.h>

#include "Win32.h"

using namespace Geo;

namespace {

    HANDLE s_hStdErr = nullptr;
    LogLevel s_maxLogLevel = LogLevel(LOG_LEVEL_DEFAULT);

    void WriteConsole(HANDLE hConsoleOutput, std::wstring_view str)
    {
        DWORD nCharsWritten;

        while (!str.empty()) {
            if (!::WriteConsoleW(hConsoleOutput, str.data(), DWORD(str.length()), &nCharsWritten, nullptr))
                break;
            else if (nCharsWritten == 0)
                break;

            str = str.substr(nCharsWritten);
        }
    }

    struct LoggerState {
        HANDLE hMutex = nullptr;
        bool locked = false; // Prevents recursive log messages
        std::wstring fatalMsg;

        static LoggerState& GetInstance()
        {
            static LoggerState instance;
            return instance;
        }

    private:
        LoggerState()
        {
            hMutex = CreateMutexW(nullptr, false, nullptr);
            if (!hMutex) {
                auto msg = Format::String(L"CreateMutexW failed: {}", Win32::GetErrorStringW(GetLastError()));
                Win32::ShowMessageBox(nullptr, msg.c_str(), L"Error", MB_OK | MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }
        }

        LoggerState(const LoggerState&) = delete;
        ~LoggerState() = default;

        LoggerState& operator=(const LoggerState&) = delete;
    };

} // namespace

void Debug::EnableConsole()
{
    if (!s_hStdErr && AllocConsole()) {
        s_hStdErr = GetStdHandle(STD_ERROR_HANDLE);
        SetConsoleMode(s_hStdErr, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
    }
}

void Debug::SetMaxLogLevel(LogLevel level)
{
    s_maxLogLevel = Math::Clamp(level, LogLevel::Off, LogLevel(LOG_LEVEL_MAX));
}

bool Debug::Internal::BeginLogMessage(LogLevel level)
{
    const wchar_t* prefix;
    WORD prefixAttrib;
    WORD msgAttrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    if (!s_hStdErr)
        return false;
    else if (level < LogLevel::Fatal || level > s_maxLogLevel)
        return false;

    switch (level) {
#if LOG_LEVEL_MAX >= LOG_LEVEL_ERROR
        case LogLevel::Error:
            prefix = L"ERROR: ";
            prefixAttrib = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_WARNING
        case LogLevel::Warning:
            prefix = L"WARNING: ";
            prefixAttrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_INFO
        case LogLevel::Info:
            prefix = L"INFO: ";
            prefixAttrib = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_DEBUG
        case LogLevel::Debug:
            prefix = L"DEBUG: ";
            prefixAttrib = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            msgAttrib = FOREGROUND_GREEN;
            break;
#endif
#if LOG_LEVEL_MAX >= LOG_LEVEL_TRACE
        case LogLevel::Trace:
            prefix = L"TRACE: ";
            prefixAttrib = FOREGROUND_GREEN | FOREGROUND_BLUE;
            msgAttrib = FOREGROUND_INTENSITY;
            break;
#endif
        default:
            return false;
    }

    auto& logger = LoggerState::GetInstance();

    // Lock the logger mutex
    switch (WaitForSingleObject(logger.hMutex, INFINITE)) {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            break;
        default: {
            auto msg = Format::String(L"WaitForSingleObject failed: {}", Win32::GetErrorStringW(GetLastError()));
            Win32::ShowMessageBox(nullptr, msg.c_str(), L"Error", MB_OK | MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
            break;
        }
    }

    // Prevent recursive messages
    if (logger.locked)
        return false;

    SetConsoleTextAttribute(s_hStdErr, prefixAttrib);
    WriteConsole(s_hStdErr, prefix);
    SetConsoleTextAttribute(s_hStdErr, msgAttrib);
    return true;
}

void Debug::Internal::WriteLogMessage(OsStringView str)
{
    WriteConsole(s_hStdErr, str);
}

void Debug::Internal::EndLogMessage()
{
    EndLogMessage(nullptr, 0);
}

void Debug::Internal::EndLogMessage(const oschar_t* file, int line)
{
    auto& logger = LoggerState::GetInstance();

    if (file) {
        SetConsoleTextAttribute(s_hStdErr, FOREGROUND_INTENSITY);
        Format::Write(&WriteLogMessage, L" ({}:{})", file, line);
    }

    WriteConsole(s_hStdErr, L"\r\n");
    ReleaseMutex(logger.hMutex);
}

void Debug::Internal::BeginFatalError()
{
    auto& logger = LoggerState::GetInstance();

    // Lock the logger mutex
    switch (WaitForSingleObject(logger.hMutex, INFINITE)) {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            break;
        default:
            Win32::ShowMessageBox(nullptr, L"WaitForSingleObject failed", L"Error", MB_OK | MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
            break;
    }

    // Check if we're interrupting another message
    if (logger.locked) {
        WriteConsole(s_hStdErr, L"\r\n");
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

    SetConsoleTextAttribute(s_hStdErr, FOREGROUND_RED | FOREGROUND_INTENSITY);
    WriteConsole(s_hStdErr, L"FATAL ERROR: ");
    SetConsoleTextAttribute(s_hStdErr, FOREGROUND_RED);
    WriteConsole(s_hStdErr, logger.fatalMsg);

    if (file) {
        auto callback = [&logger](std::wstring_view str) {
            WriteConsole(s_hStdErr, str);
            logger.fatalMsg += str;
        };
        SetConsoleTextAttribute(s_hStdErr, FOREGROUND_INTENSITY);
        Format::Write(callback, L" ({}:{})", file, line);
    }

    WriteConsole(s_hStdErr, L"\r\n");
    Win32::ShowMessageBox(nullptr, logger.fatalMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
    ExitFatal();
}

void Debug::Internal::ExitFatal()
{
    ExitProcess(EXIT_FAILURE);
}
