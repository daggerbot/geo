/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <filesystem>
#include <memory>

#include <Core/GameDefs.h>
#include <System/Debug.h>
#include <System/System.h>

#include "Win32.h"

using namespace Geo;

namespace {

    std::wstring GetExePath()
    {
        DWORD bufferSize = PATH_MAX;
        std::unique_ptr<wchar_t[]> buffer{new wchar_t[bufferSize]};
        DWORD result;
        DWORD errnum;

        SetLastError(0);

        for (;;) {
            result = GetModuleFileNameW(nullptr, buffer.get(), bufferSize);
            errnum = GetLastError();

            if (errnum && errnum != ERROR_INSUFFICIENT_BUFFER)
                FATAL("GetModuleFileNameW: {}", Win32::GetErrorStringW(errnum));

            if (result > 0 && result < bufferSize - 1)
                return {buffer.get(), result};

            bufferSize *= 2;
            buffer.reset(new wchar_t[bufferSize]);
        }
    }

    std::wstring GetExeDir()
    {
        return std::filesystem::path{GetExePath()}.parent_path().wstring();
    }

} // namespace

OsString System::GetDefaultAssetPakPath()
{
    return GetExeDir() + L"\\" PAK_FILENAME;
}
