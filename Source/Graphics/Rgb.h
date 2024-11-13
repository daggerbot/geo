/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_GRAPHICS_RGB_H_INCLUDED
#define GEO_GRAPHICS_RGB_H_INCLUDED

#include "Color.h"

namespace Geo {

    /// @addtogroup Graphics
    /// @{

    /// Red-green-blue color type.
    template<typename T>
    struct Rgb {
        T r, g, b;

        static constexpr Rgb<T> FromArgb(u32 color)
        requires ColorComponent<T>
        {
            return {
                .r = Color::GetRed<T>(color),
                .g = Color::GetGreen<T>(color),
                .b = Color::GetBlue<T>(color),
            };
        }
    };

    using Rgb8 = Rgb<u8>;
    using Rgbf = Rgb<f32>;

    /// Red-green-blue-alpha color type.
    template<typename T>
    struct Rgba {
        T r, g, b, a;

        static constexpr Rgba<T> FromArgb(u32 color)
        requires ColorComponent<T>
        {
            return {
                .r = Color::GetRed<T>(color),
                .g = Color::GetGreen<T>(color),
                .b = Color::GetBlue<T>(color),
                .a = Color::GetAlpha<T>(color),
            };
        }
    };

    using Rgba8 = Rgba<u8>;
    using Rgbaf = Rgba<f32>;

    /// @}

} // namespace Geo

#endif // GEO_GRAPHICS_RGB_H_INCLUDED
