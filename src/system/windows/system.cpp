/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <filesystem>

#include <core/game_defs.h>
#include <system/debug.h>
#include <system/system.h>

#include "win32.h"

using namespace geo;

namespace {

    std::wstring get_exe_path()
    {
        DWORD buf_size = MAX_PATH;
        std::unique_ptr<wchar_t[]> buf{new wchar_t[buf_size]};
        DWORD result;
        DWORD errnum;

        SetLastError(0);

        for (;;) {
            result = GetModuleFileNameW(nullptr, buf.get(), buf_size);
            errnum = GetLastError();

            if (errnum && errnum != ERROR_INSUFFICIENT_BUFFER)
                FATAL("GetModuleFileNameW failed: {}", win32::wcserror(errnum));
            else if (result && result < buf_size - 1)
                return buf.get();

            buf_size *= 2;
            buf.reset(new wchar_t[buf_size]);
        }
    }

    std::wstring get_exe_dir()
    {
        return std::filesystem::path{get_exe_path()}.parent_path().wstring();
    }

} // namespace

OsString system::get_default_pak_path()
{
    return get_exe_dir() + L"\\" PAK_FILENAME;
}
