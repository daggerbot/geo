/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CORE_STR_H_INCLUDED
#define CORE_STR_H_INCLUDED

#include "concepts.h"

namespace geo {

    /// String utility functions.
    namespace str {

        /// Finds the first occurrence of `ch` in `str`. If `ch` is zero, this finds the terminating
        /// null character. Returns null if `ch` is not found in `str`.
        template<CoreInt CharT, CoreInt Ch>
        constexpr CharT* find(CharT* str, Ch ch)
        requires Equatable<CharT, Ch>
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
        template<CoreInt CharT>
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

#endif // CORE_STR_H_INCLUDED
