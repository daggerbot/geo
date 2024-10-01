/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_STATIC_STRING_H_INCLUDED
#define GEO_CORE_STATIC_STRING_H_INCLUDED

#include <string_view>

#include "concepts.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Static string type that can be used for string literal template arguments.
    template<CoreInt CharT, size_t N>
    struct StaticString {
        using char_type = CharT;

        static constexpr size_t size = N;

        CharT data[N];

        constexpr StaticString()
        {
            for (size_t i = 0; i < N; ++i)
                data[i] = 0;
        }

        constexpr StaticString(const StaticString<CharT, N>& other)
        {
            for (size_t i = 0; i < N; ++i)
                data[i] = other.data[i];
        }

        constexpr StaticString(const CharT (&data)[N])
        {
            for (size_t i = 0; i < N; ++i)
                this->data[i] = data[i];
        }

        constexpr StaticString<CharT, N>& operator=(const StaticString<CharT, N>& other) = default;

        template<size_t M>
        constexpr bool operator==(const StaticString<CharT, M>& rhs) const
        {
            return *this == rhs.data;
        }

        template<size_t M>
        constexpr bool operator==(const CharT (&rhs)[M]) const
        {
            if constexpr(N != M)
                return false;

            for (size_t i = 0; i < N; i++)
                if (data[i] != rhs[i])
                    return false;

            return true;
        }

        /// Accesses a character without bounds checking.
        constexpr CharT& operator[](size_t index) { return data[index]; }

        /// Accesses a character without bounds checking.
        constexpr const CharT& operator[](size_t index) const { return data[index]; }

        /// Returns a `std::basic_string_view` over all of the static string's data, including the
        /// terminating null character if present.
        constexpr std::basic_string_view<CharT> full_view() const
        {
            return {data, N};
        }

        /// Returns a substring.
        template<size_t Index, size_t Length = N - Index>
        constexpr auto substr() const
        requires (Index < N && Length > 0 && Length <= N - Index)
        {
            StaticString<CharT, Length> substr;

            for (size_t i = 0; i < Length; ++i)
                substr[i] = data[Index + i];

            return substr;
        }

        /// Returns a `std::basic_string_view` over all of the static string's data, except the
        /// terminating null character if present.
        constexpr std::basic_string_view<CharT> unterminated_view() const
        {
            return {data, data[N - 1] ? N : N - 1};
        }
    };

    /// @}

} // namespace geo

#endif // GEO_CORE_STATIC_STRING_H_INCLUDED
