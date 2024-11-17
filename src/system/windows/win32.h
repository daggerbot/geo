/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_WINDOWS_WIN32_H_INCLUDED
#define SYSTEM_WINDOWS_WIN32_H_INCLUDED

#include <wtypes.h>

#include <string>

#include <system/system.h>

namespace geo {

    /// Win32 system functions.
    namespace win32 {

        using MessageBoxProc = int (WINAPI*)(HWND, LPCWSTR, LPCWSTR, UINT);

        std::string strerror(DWORD errnum);
        std::wstring wcserror(DWORD errnum);

        /// Invokes the message box handler.
        int message_box(HWND hWnd, LPCWSTR lpMessage, LPCWSTR lpTitle, UINT flags);

        /// Sets the message box handler. This must be done manually by GUI applications so headless
        /// applications, such as a dedicated server, can remove all dependence on `user32.dll` and
        /// avoid making any GUI-related calls.
        void set_message_box_proc(MessageBoxProc proc);

    } // namespace win32

} // namespace geo

#endif // SYSTEM_WINDOWS_WIN32_H_INCLUDED
