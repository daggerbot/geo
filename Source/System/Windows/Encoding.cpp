/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <System/Debug.h>
#include <System/Encoding.h>

#include "Win32.h"

using namespace Geo;

std::wstring Encoding::SystemNarrowToWide(std::string_view str)
{
    std::wstring wstr;
    int result, result2;

    if (str.empty())
        return wstr;

    result = MultiByteToWideChar(CP_ACP, 0, str.data(), int(str.length()), nullptr, 0);

    if (result < 1)
        FATAL("MultiByteToWideChar failed: {}", Win32::GetErrorStringW(GetLastError()));

    wstr.resize(size_t(result));
    result2 = MultiByteToWideChar(CP_ACP, 0, str.data(), int(str.length()), wstr.data(), result + 1);

    if (result2 < 1 || result2 > result)
        FATAL("MultiByteToWideChar failed: {}", Win32::GetErrorStringW(GetLastError()));

    wstr.resize(size_t(result2));
    return wstr;
}
