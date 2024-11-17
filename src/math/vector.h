/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MATH_VECTOR_H_INCLUDED
#define MATH_VECTOR_H_INCLUDED

#include <core/types.h>

namespace geo {

    /// 2-dimensional vector type.
    template<typename T>
    struct Vec2 {
        T x, y;
    };

    using Vec2i = Vec2<i32>;
    using Vec2f = Vec2<f32>;

    /// 3-dimensional vector type.
    template<typename T>
    struct Vec3 {
        T x, y, z;
    };

    using Vec3i = Vec3<i32>;
    using Vec3f = Vec3<f32>;

    // 4-dimensional vector type.
    template<typename T>
    struct Vec4 {
        T x, y, z, w;
    };

    using Vec4i = Vec4<i32>;
    using Vec4f = Vec4<f32>;

} // namespace geo

#endif // MATH_VECTOR_H_INCLUDED
