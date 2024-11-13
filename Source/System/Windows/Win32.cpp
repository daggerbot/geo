/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <Core/Format.h>

#include "Win32.h"

using namespace Geo;

namespace {

    Win32::MessageBoxProc s_messageBoxHandler = nullptr;

} // namespace

std::wstring Win32::GetErrorStringW(DWORD errnum)
{
    LPWSTR msgPtr = nullptr;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errnum, 0,
                   reinterpret_cast<LPWSTR>(&msgPtr), 0, nullptr);

    if (msgPtr) {
        std::wstring msg = msgPtr;
        LocalFree(msgPtr);
        return msg;
    } else {
        return Format::String(L"Win32 error code {}", errnum);
    }
}

void Win32::SetMessageBoxHandler(MessageBoxProc proc)
{
    s_messageBoxHandler = proc;
}

int Win32::ShowMessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    if (s_messageBoxHandler) {
        return s_messageBoxHandler(hWnd, lpText, lpCaption, uType);
    } else {
        SetLastError(ERROR_REQUIRES_INTERACTIVE_WINDOWSTATION);
        return 0;
    }
}
