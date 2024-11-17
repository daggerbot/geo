/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MATH_RECT_H_INCLUDED
#define MATH_RECT_H_INCLUDED

#include "vector.h"

namespace geo {

    /// Rectangle defined as two opposing corner points.
    template<typename T>
    struct Rect {
        T x0, y0, x1, y1;

        constexpr auto height() const
        {
            return y1 - y0;
        }

        constexpr auto size() const
        {
            return Vec2{width(), height()};
        }

        constexpr auto width () const
        {
            return x1 - x0;
        }
    };

    using Recti = Rect<i32>;
    using Rectf = Rect<f32>;

} // namespace geo

#endif // MATH_RECT_H_INCLUDED
