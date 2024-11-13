/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_MATH_MATH_H_INCLUDED
#define GEO_MATH_MATH_H_INCLUDED

#include <Core/Concepts.h>

namespace Geo {

    /// @addtogroup Math
    /// @{

    /// Math functions.
    namespace Math {

        /// Clamps `x` between `min` and `max`. The return value is unspecified if `min` is not less
        /// than or equal to `max`.
        template<PartiallyOrdered T>
        constexpr T Clamp(T x, T min, T max)
        {
            if (x < min)
                return min;
            else if (x > max)
                return max;
            else
                return x;
        }

        /// Returns the greater of two values.
        template<PartiallyOrdered T>
        constexpr T Max(T x, T y)
        {
            return y > x ? y : x;
        }

        /// Returns the lesser of two values.
        template<PartiallyOrdered T>
        constexpr T Min(T x, T y)
        {
            return y < x ? y : x;
        }

    } // namespace Math

    /// @}

} // namespace Geo

#endif // GEO_MATH_MATH_H_INCLUDE
