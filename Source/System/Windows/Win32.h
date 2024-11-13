/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_SYSTEM_WINDOWS_WIN32_H_INCLUDED
#define GEO_SYSTEM_WINDOWS_WIN32_H_INCLUDED

#include <windows.h>

#include <string>

#include <Core/Types.h>

namespace Geo {

    /// @addtogroup System
    /// @{

    /// Win32-specific system functions.
    namespace Win32 {

        using MessageBoxProc = int (WINAPI*)(HWND, LPCWSTR, LPCWSTR, UINT);

        /// Returns a string describing a Win32 error code.
        std::wstring GetErrorStringW(DWORD errnum);

        /// Sets the message box handler. Parts of the game engine use @ref ShowMessageBox to report
        /// errors, but we don't want to actually show a message box in headless applications, such
        /// as a dedicated server.
        void SetMessageBoxHandler(MessageBoxProc proc);

        /// Invokes the message box handler. If no message box handler was registered, this returns
        /// 0 and sets the error code to `ERROR_REQUIRES_INTERACTIVE_WINDOWSTATION`.
        /// @see @ref SetMessageBoxHandler
        int ShowMessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

    } // namespace Win32

    /// @}

} // namespace Geo

#endif // GEO_SYSTEM_WINDOWS_WIN32_H_INCLUDED
