/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_MATH_RECT_H_INCLUDED
#define GEO_MATH_RECT_H_INCLUDED

#include <Core/Concepts.h>

#include "Vector.h"

namespace Geo {

    /// @addtogroup Math
    /// @{

    /// Rectangle defined by two opposing corner points. Many `Rect` operations assume that
    /// `x1 >= x0 && y1 >= y0`.
    template<typename T>
    struct Rect {
        T x0, y0, x1, y1;

        /// Returns the rectangle's height, assuming `y1 >= y0`.
        constexpr auto GetHeight() const
        requires CoreScalar<T>
        {
            return y1 - y0;
        }

        /// Returns the rectangle's dimensions as width and height, assuming `x1 >= x0 && y1 >= y0`.
        constexpr auto GetSize() const
        requires CoreScalar<T>
        {
            return Vec2{x1 - x0, y1 - y0};
        }

        /// Returns the rectangle's width, assuming `x1 >= x0`.
        constexpr auto GetWidth() const
        requires CoreScalar<T>
        {
            return x1 - x0;
        }
    };

    using Recti = Rect<i32>;
    using Rectf = Rect<f32>;

    /// @}

} // namespace Geo

#endif // GEO_MATH_RECT_H_INCLUDED
