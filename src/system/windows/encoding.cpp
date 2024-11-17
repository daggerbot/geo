/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <math/math.h>
#include <system/encoding.h>

using namespace geo;

std::wstring encoding::system_to_wide(std::string_view str)
{
    std::wstring out_str;
    int result;

    if (str.empty())
        return out_str;

    result = std::max(MultiByteToWideChar(CP_ACP, 0, str.data(), int(str.length()), nullptr, 0), 0);
    out_str.resize(size_t(result));
    result = std::max(MultiByteToWideChar(CP_ACP, 0, str.data(), int(str.length()), out_str.data(), result + 1), 0);
    out_str.resize(size_t(result));
    return out_str;
}
