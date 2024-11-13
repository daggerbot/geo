/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_ARRAY_H_INCLUDED
#define GEO_CORE_ARRAY_H_INCLUDED

#include "Concepts.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// Utilities for working with arrays.
    namespace Array {

        /// Copies `n` elements from `src` to `dst`.
        template<typename Src, typename Dst>
        constexpr void Copy(const Src* src, Dst* dst, size_t n)
        requires (std::is_assignable_v<Dst&, const Src&>)
        {
            for (size_t i = 0; i < n; i++)
                dst[i] = src[i];
        }

        /// Compares two spans for equality.
        template<typename L, typename R>
        constexpr bool Equals(const L* l, const R* r, size_t n)
        requires Equatable<const L&, const R&>
        {
            for (size_t i = 0; i < n; ++i)
                if (*l != *r)
                    return false;

            return true;
        }

        /// Gets the length of an array.
        template<typename T, size_t N>
        consteval size_t Length(const T (&)[N])
        {
            return N;
        }

        /// Gets the length of an array type.
        template<typename T>
        consteval size_t Length()
        requires (std::is_array_v<T>)
        {
            return std::extent_v<T>;
        }

    } // namespace Array

    /// @}

} // namespace Geo

#endif // GEO_CORE_ARRAY_H_INCLUDED
