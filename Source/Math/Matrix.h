/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_MATH_MATRIX_H_INCLUDED
#define GEO_MATH_MATRIX_H_INCLUDED

#include "Vector.h"

namespace Geo {

    /// @addtogroup Math
    /// @{

    /// 4x4 matrix type.
    template<typename T>
    struct Mat4 {
        Vec4<T> x, y, z, w;

        /// Constructs an identity matrix.
        static constexpr Mat4<T> Identity()
        {
            return {
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1},
            };
        }
    };

    using Mat4f = Mat4<f32>;

    /// @}

} // namespace Geo

#endif // GEO_MATH_MATRIX_H_INCLUDED
