/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_ENCODING_H_INCLUDED
#define SYSTEM_ENCODING_H_INCLUDED

#include <string>

#include <fmt/format.h>
#include <fmt/xchar.h>

#include <core/types.h>

namespace geo {

    /// Text conversion functions.
    namespace encoding {

        std::wstring system_to_wide(std::string_view str);

    } // namespace encoding

    /// Object that widens system-narrow-encoded strings when formatted.
    struct Widen {
        std::string_view str;
    };

} // namespace geo

template<>
struct fmt::formatter<geo::Widen, wchar_t> : formatter<std::wstring_view, wchar_t> {
    wformat_context::iterator format(const geo::Widen& widen, wformat_context& ctx) const
    {
        return formatter<std::wstring_view, wchar_t>::format(geo::encoding::system_to_wide(widen.str), ctx);
    }
};

#endif // SYSTEM_ENCODING_H_INCLUDED
