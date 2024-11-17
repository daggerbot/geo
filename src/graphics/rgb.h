/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GRAPHICS_RGB_H_INCLUDED
#define GRAPHICS_RGB_H_INCLUDED

#include <core/types.h>

namespace geo {

    /// Red-green-blue color type.
    template<typename T>
    struct Rgb {
        T r, g, b;
    };

    using Rgb8 = Rgb<u8>;
    using Rgbf = Rgb<f32>;

    /// Red-green-blue-alpha color type.
    template<typename T>
    struct Rgba {
        T r, g, b, a;
    };

    using Rgba8 = Rgba<u8>;
    using Rgbaf = Rgba<f32>;

} // namespace geo

#endif // GRAPHICS_RGB_H_INCLUDED
