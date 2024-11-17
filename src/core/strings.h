/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CORE_STRINGS_H_INCLUDED
#define CORE_STRINGS_H_INCLUDED

#include <string>

#include "types.h"

namespace geo {

    using OsString = std::basic_string<OsChar>;
    using OsStringView = std::basic_string_view<OsChar>;

    /// String utility functions.
    namespace str {

        /// Finds the first occurrence of `ch` in `str`. If `ch` is zero, this finds the terminating
        /// null character. Returns null if `ch` is not found in `str`.
        template<typename CharT>
        constexpr const CharT* find(const CharT* str, CharT ch)
        {
            if (!str)
                return nullptr;

            for (;;) {
                if (*str == ch)
                    return str;
                else if (*str == 0)
                    return nullptr;
                else
                    ++str;
            }
        }

        /// Gets the length of a null-terminated string.
        template<typename CharT>
        constexpr size_t length(const CharT* str)
        {
            size_t len = 0;

            if (!str)
                return 0;

            while (str[len])
                ++len;

            return len;
        }

    } // namespace str

} // namespace geo

#endif // CORE_STRINGS_H_INCLUDED
