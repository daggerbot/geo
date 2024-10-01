/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_STRING_UTILS_H_INCLUDED
#define GEO_CORE_STRING_UTILS_H_INCLUDED

#include <string_view>

#include "concepts.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// String utility functions.
    namespace string_utils {

        /// Compares two null-terminated strings for equality.
        template<CoreInt CharT>
        constexpr bool equals(const CharT* lhs, const CharT* rhs)
        {
            if (!lhs || !rhs)
                return lhs == rhs;
            else if (lhs == rhs)
                return true;

            for (;;) {
                if (*lhs != *rhs)
                    return false;
                else if (*lhs == 0)
                    return true;

                ++lhs;
                ++rhs;
            }
        }

        /// Gets the length of a possibly null-terminated string up to a specified maximum. This is
        /// a generic implementation of `strnlen`.
        ///
        /// @return Index of the first nul character in `str`, but no greater than `max`. Returns
        ///   zero if `str` is null.
        template<CoreInt CharT>
        constexpr size_t length_with_max(const CharT* str, size_t max)
        {
            size_t length = 0;

            if (!str)
                return 0;

            while (length < max && str[length])
                ++length;

            return length;
        }

        /// Constructs a `std::basic_string_view` from a potentially null-terminated string, limited
        /// by a maximum length.
        ///
        /// @return A string view up to the first null character in `str`, or up to `max`
        ///   characters. Returns an empty string view if `str` is null.
        template<CoreInt CharT>
        constexpr std::basic_string_view<CharT> view_with_max(const CharT* str, size_t max)
        {
            return std::basic_string_view<CharT>{str, string_utils::length_with_max(str, max)};
        }

        /// Constructs a `std::basic_string_view` from a character array, stopping at the first null
        /// character.
        ///
        /// @return A string view of the array, truncated at the first null character if present.
        template<CoreInt CharT, size_t N>
        constexpr std::basic_string_view<CharT> view_truncated(const CharT (&str)[N])
        {
            return string_utils::view_with_max(str, N);
        }

    } // namespace string_utils

    /// @}

} // namespace geo

#endif // GEO_CORE_STRING_UTILS_H_INCLUDED
