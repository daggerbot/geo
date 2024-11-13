/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_STATICSTRING_H_INCLUDED
#define GEO_CORE_STATICSTRING_H_INCLUDED

#include <string_view>

#include "Array.h"
#include "Concepts.h"
#include "Str.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// String type with a size determined at compile time. Typically represents a string literal
    /// template argument.
    template<CoreInt CharT, size_t N>
    struct StaticString {
        using CharType = CharT;

        /// Length of the string, including the terminating null character if present.
        static constexpr size_t Size = N;

        CharT data[N];

        constexpr StaticString() : data{0} {}
        constexpr StaticString(const StaticString<CharT, N>& other) = default;

        /// Constructs the static string from a character array.
        constexpr StaticString(const CharT (&data)[N])
        {
            Array::Copy(data, this->data, N);
        }

        constexpr StaticString<CharT, N>& operator=(const StaticString<CharT, N>& other) = default;

        /// Compares two static strings for equality. Any terminating null characters are included
        /// in the comparison.
        template<size_t Rn>
        constexpr bool operator==(const StaticString<CharT, Rn>& rhs) const
        {
            return *this == rhs.data;
        }

        /// Compares the static string to a character array for equality. Any terminating null
        /// characters are included in the comparison.
        template<size_t Rn>
        constexpr bool operator==(const CharT (&rhs)[Rn]) const
        {
            if constexpr(Rn != N)
                return false;

            return Array::Equals(data, rhs, N);
        }

        /// Gets a reference to the character at `index`, without bounds checking.
        constexpr CharT& operator[](size_t index) { return data[index]; }

        /// Gets a reference to the character at `index`, without bounds checking.
        constexpr const CharT& operator[](size_t index) const { return data[index]; }

        /// Returns a view over the entire string, including the terminating null character if
        /// present.
        constexpr std::basic_string_view<CharT> GetFullView() const
        {
            return {data, N};
        }

        /// Returns a view over the string up to the first null character.
        constexpr std::basic_string_view<CharT> GetTruncatedView() const
        {
            return Str::ViewBounded(data);
        }

        /// Returns a view over the entire string, excluding the last character if it is zero.
        constexpr std::basic_string_view<CharT> GetUnterminatedView() const
        {
            return {data, data[N - 1] ? N : N - 1};
        }
    };

    /// @}

} // namespace Geo

#endif // GEO_CORE_STATICSTRING_H_INCLUDED
