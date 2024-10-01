/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_GRAPHICS_COLOR_H_INCLUDED
#define GEO_GRAPHICS_COLOR_H_INCLUDED

#include <geo/core/types.h>

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Common colors.
    namespace colors {

        inline constexpr u32 transparent = 0;
        inline constexpr u32 black = 0xff000000;
        inline constexpr u32 white = 0xffffffff;

    } // namespace colors

    /// Type traits for color component types.
    template<typename T>
    struct ColorComponentTraits;

    template<>
    struct ColorComponentTraits<u8> {
        static constexpr u8 from_u8(u8 value) { return value; }
        static constexpr u8 to_u8(u8 value) { return value; }
    };

    template<>
    struct ColorComponentTraits<f32> {
        static constexpr f32 from_u8(u8 value) { return f32(value) / 255.0f; }
        static constexpr u8 to_u8(f32 value) { return u8(value / 255.0f); }
    };

    template<>
    struct ColorComponentTraits<f64> {
        static constexpr f64 from_u8(u8 value) { return f64(value) / 255.0; }
        static constexpr u8 to_u8(f64 value) { return u8(value / 255.0); }
    };

    /// Concept for types with a specialization of @a ColorComponentTraits.
    template<typename T>
    concept ColorComponent = requires {
        { ColorComponentTraits<T>{} };
    };

    /// Red-green color type.
    template<typename T>
    struct Rg {
        T r, g;

        constexpr Rg() = default;
        constexpr Rg(const Rg<T>& other) = default;

        constexpr Rg(const T& r, const T& g)
            : r{r}, g{g}
        {
        }

        /// Constructs a color from an `0xAARRGGBB` value.
        constexpr Rg(u32 argb)
        requires ColorComponent<T>
            : r{ColorComponentTraits<T>::from_u8(u8(argb >> 16))}
            , g{ColorComponentTraits<T>::from_u8(u8(argb >> 8))}
        {
        }

        constexpr Rg<T>& operator=(const Rg<T>& other) = default;
    };

    using Col2b = Rg<u8>;
    using Col2f = Rg<f32>;
    using Col2d = Rg<f64>;

    /// Red-green-blue color type.
    template<typename T>
    struct Rgb {
        T r, g, b;

        constexpr Rgb() = default;
        constexpr Rgb(const Rgb<T>& other) = default;

        constexpr Rgb(const T& r, const T& g, const T& b)
            : r{r}, g{g}, b{b}
        {
        }

        /// Constructs a color from an `0xAARRGGBB` value.
        constexpr Rgb(u32 argb)
        requires ColorComponent<T>
            : r{ColorComponentTraits<T>::from_u8(u8(argb >> 16))}
            , g{ColorComponentTraits<T>::from_u8(u8(argb >> 8))}
            , b{ColorComponentTraits<T>::from_u8(u8(argb))}
        {
        }

        constexpr Rgb<T>& operator=(const Rgb<T>& other) = default;
    };

    using Col3b = Rgb<u8>;
    using Col3f = Rgb<f32>;
    using Col3d = Rgb<f64>;

    /// Red-green-blue-alpha color type.
    template<typename T>
    struct Rgba {
        T r, g, b, a;

        constexpr Rgba() = default;
        constexpr Rgba(const Rgba<T>& other) = default;

        constexpr Rgba(const T& r, const T& g, const T& b, const T& a)
            : r{r}, g{g}, b{b}, a{a}
        {
        }

        /// Constructs a color from an `0xAARRGGBB` value.
        constexpr Rgba(u32 argb)
        requires ColorComponent<T>
            : r{ColorComponentTraits<T>::from_u8(u8(argb >> 16))}
            , g{ColorComponentTraits<T>::from_u8(u8(argb >> 8))}
            , b{ColorComponentTraits<T>::from_u8(u8(argb))}
            , a{ColorComponentTraits<T>::from_u8(u8(argb >> 24))}
        {
        }

        constexpr Rgba<T>& operator=(const Rgba<T>& other) = default;
    };

    using Col4b = Rgba<u8>;
    using Col4f = Rgba<f32>;
    using Col4d = Rgba<f64>;

    /// @}

} // namespace geo

#endif // GEO_GRAPHICS_COLOR_H_INCLUDED
