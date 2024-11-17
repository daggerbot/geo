/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MATH_MATRIX_H_INCLUDED
#define MATH_MATRIX_H_INCLUDED

#include "vector.h"

namespace geo {

    /// 4x4 matrix type.
    template<typename T>
    struct Mat4 {
        Vec4<T> x, y, z, w;

        /// Returns an identity matrix.
        static constexpr Mat4<T> identity()
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

} // namespace geo

#endif // MATH_MATRIX_H_INCLUDED
