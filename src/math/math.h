/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_MATH_MATH_H_INCLUDED
#define GEO_MATH_MATH_H_INCLUDED

#include <core/types.h>

namespace geo {

    /// Math utility functions.
    namespace math {

        /// Clamps `x` between `min` and `max`.
        template<typename T>
        constexpr T clamp(T x, T min, T max)
        {
            if (x < min)
                return min;
            else if (x > max)
                return max;
            else
                return x;
        }

        /// Returns the greater of two values.
        template<typename T>
        constexpr T max(T x, T y)
        {
            return y > x ? y : x;
        }

        /// Returns the lesser of two values.
        template<typename T>
        constexpr T min(T x, T y)
        {
            return y < x ? y : x;
        }

    } // namespace math

} // namespace geo

#endif // GEO_MATH_MATH_H_INCLUDED
