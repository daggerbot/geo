/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_GRAPHICS_COLOR_H_INCLUDED
#define GEO_GRAPHICS_COLOR_H_INCLUDED

#include <Core/Types.h>

namespace Geo {

    /// @addtogroup Graphics
    /// @{

    /// Type traits for color component types.
    template<typename T>
    struct ColorComponentTraits;

    template<>
    struct ColorComponentTraits<u8> {
        static constexpr u8 Zero = 0;
        static constexpr u8 Full = 0xff;

        static constexpr u8 FromByte(u8 byte) { return byte; }
        static constexpr u8 ToByte(u8 x) { return x; }
    };

    template<>
    struct ColorComponentTraits<f32> {
        static constexpr f32 Zero = 0.0f;
        static constexpr f32 Full = 1.0f;

        static constexpr f32 FromByte(u8 byte) { return f32(byte) / 255.0f; }
        static constexpr u8 ToByte(f32 x) { return u8(x * 255.0f); }
    };

    /// Concept for color component types. This is determined by the presence of a proper
    /// specialization of @ref ColorComponentTraits.
    template<typename T>
    concept ColorComponent =
        requires {
            { ColorComponentTraits<T>::Zero } -> std::same_as<const T&>;
            { ColorComponentTraits<T>::Full } -> std::same_as<const T&>;
        } && requires(u8 byte) {
            { ColorComponentTraits<T>::FromByte(byte) } -> std::same_as<T>;
        } && requires(T x) {
            { ColorComponentTraits<T>::ToByte(x) } -> std::same_as<u8>;
        };

    /// `0xAARRGGBB` color constants, and functions for working with them.
    namespace Color {

        static inline constexpr u32 Transparent = 0;

        static inline constexpr u32 Black = 0xff000000;
        static inline constexpr u32 DarkGray = 0xff404040;
        static inline constexpr u32 Gray = 0xff808080;
        static inline constexpr u32 LightGray = 0xffc0c0c0;
        static inline constexpr u32 White = 0xffffffff;

        static inline constexpr u32 Red = 0xffff0000;
        static inline constexpr u32 Yellow = 0xffffff00;
        static inline constexpr u32 Green = 0xff00ff00;
        static inline constexpr u32 Cyan = 0xff00ffff;
        static inline constexpr u32 Blue = 0xff0000ff;
        static inline constexpr u32 Magenta = 0xffff00ff;

        template<ColorComponent T>
        constexpr T GetAlpha(u32 color)
        {
            return ColorComponentTraits<T>::FromByte(u8(color >> 24));
        }

        template<ColorComponent T>
        constexpr T GetRed(u32 color)
        {
            return ColorComponentTraits<T>::FromByte(u8(color >> 16));
        }

        template<ColorComponent T>
        constexpr T GetGreen(u32 color)
        {
            return ColorComponentTraits<T>::FromByte(u8(color >> 8));
        }

        template<ColorComponent T>
        constexpr T GetBlue(u32 color)
        {
            return ColorComponentTraits<T>::FromByte(u8(color));
        }

    } // namespace Color

    /// @}

} // namespace Geo

#endif // GEO_GRAPHICS_COLOR_H_INCLUDED
