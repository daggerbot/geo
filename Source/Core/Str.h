/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_STR_H_INCLUDED
#define GEO_CORE_STR_H_INCLUDED

#include <string_view>

#include "Concepts.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// String utility functions.
    namespace Str {

        /// Finds the first occurrence of `ch` in `str`. If `ch` is zero, this finds the terminating
        /// null character.
        template<CoreInt CharT>
        constexpr CharT* Find(CharT* str, std::remove_const_t<CharT> ch)
        {
            if (!str)
                return nullptr;

            for (;;) {
                if (*str == ch)
                    return str;
                else if (ch == 0)
                    return nullptr;
                else
                    ++str;
            }
        }

        /// Gets the length of a null-terminated string.
        template<CoreInt CharT>
        constexpr size_t Length(const CharT* str)
        {
            size_t len = 0;

            if (str)
                while (str[len])
                    ++len;

            return len;
        }

        /// Gets the length of a possibly null-terminated string with a specified maximum length.
        template<CoreInt CharT>
        constexpr size_t LengthBounded(const CharT* str, size_t max)
        {
            size_t len = 0;

            if (str)
                while (len < max && str[len])
                    ++len;

            return len;
        }

        /// Gets the length of a possibly null-terminated string within a character array.
        template<CoreInt CharT, size_t N>
        constexpr size_t LengthBounded(const CharT (&str)[N])
        {
            return LengthBounded(str, N);
        }

        /// Constructs a string view from a possibly null-terminated string with a specified maximum
        /// length.
        template<CoreInt CharT>
        constexpr std::basic_string_view<CharT> ViewBounded(const CharT* str, size_t max)
        {
            return {str, LengthBounded(str, max)};
        }

        /// Constructs a string view from a possibly null-terminated string within a character
        /// array.
        template<CoreInt CharT, size_t N>
        constexpr std::basic_string_view<CharT> ViewBounded(const CharT (&str)[N])
        {
            return ViewBounded(str, N);
        }

    } // namespace Str

    /// @}

} // namespace Geo

#endif // GEO_CORE_STR_H_INCLUDED
