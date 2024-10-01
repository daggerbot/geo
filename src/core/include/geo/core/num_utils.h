/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_NUM_UTILS_H_INCLUDED
#define GEO_CORE_NUM_UTILS_H_INCLUDED

#include "concepts.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Number utilities.
    namespace num_utils {

        /// Returns the lesser of two numbers.
        template<PartiallyOrdered T>
        constexpr T min(T x, T y)
        {
            return y >= x ? x : y;
        }

        /// Returns the greater of two numbers.
        template<PartiallyOrdered T>
        constexpr T max(T x, T y)
        {
            return y <= x ? x : y;
        }

    } // namespace num_utils

    /// @}

} // namespace geo

#endif // GEO_CORE_NUM_UTILS_H_INCLUDED
