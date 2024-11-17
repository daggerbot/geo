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

using namespace geo;

namespace {

    win32::MessageBoxProc message_box_proc = nullptr;

} // namespace

std::string win32::strerror(DWORD errnum)
{
    LPSTR msg_ptr = nullptr;

    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errnum, 0,
                   reinterpret_cast<LPSTR>(&msg_ptr), 0, nullptr);

    if (msg_ptr) {
        std::string str = msg_ptr;
        LocalFree(msg_ptr);
        return str;
    } else {
        return fmt::format("Win32 error code {}", errnum);
    }
}

std::wstring win32::wcserror(DWORD errnum)
{
    LPWSTR msg_ptr = nullptr;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errnum, 0,
                   reinterpret_cast<LPWSTR>(&msg_ptr), 0, nullptr);

    if (msg_ptr) {
        std::wstring str = msg_ptr;
        LocalFree(msg_ptr);
        return str;
    } else {
        return fmt::format(L"Win32 error code {}", errnum);
    }
}

int win32::message_box(HWND hWnd, LPCWSTR lpMessage, LPCWSTR lpTitle, UINT flags)
{
    if (message_box_proc) {
        return message_box_proc(hWnd, lpMessage, lpTitle, flags);
    } else {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
}

void win32::set_message_box_proc(MessageBoxProc proc)
{
    message_box_proc = proc;
}
