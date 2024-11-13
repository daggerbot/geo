/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_TYPES_H_INCLUDED
#define GEO_CORE_TYPES_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <utility>

/// @addtogroup Core
/// @{

/// @def OSSTR
/// Encodes the following string literal as an array of @ref Geo::oschar_t.
#ifdef _WIN32
# define OSSTR L""
#else
# define OSSTR
#endif

/// @}

namespace Geo {

    /// @addtogroup Core
    /// @{

    using max_align_t = std::max_align_t;
    using nullptr_t = std::nullptr_t;
    using ptrdiff_t = std::ptrdiff_t;
    using size_t = std::size_t;

    using i8 = std::int8_t; ///< 8-bit signed integer.
    using i16 = std::int16_t; ///< 16-bit signed integer.
    using i32 = std::int32_t; ///< 32-bit signed integer.
    using i64 = std::int64_t; ///< 64-bit signed integer.
    using iptr = std::intptr_t; ///< Pointer-sized signed integer.

    using u8 = std::uint8_t; ///< 8-bit unsigned integer.
    using u16 = std::uint16_t; ///< 16-bit unsigned integer.
    using u32 = std::uint32_t; ///< 32-bit unsigned integer.
    using u64 = std::uint64_t; ///< 64-bit unsigned integer.
    using uptr = std::uintptr_t; ///< Pointer-sized unsigned integer.

    using f32 = float; ///< 32-bit floating point type.
    using f64 = double; ///< 64-bit floating point type.

    /// @typedef oschar_t
    /// Preferred character type when using the system APIs. This is typically `char`, with the
    /// notable exception that it is `oschar_t` on Windows.
#ifdef _WIN32
    using oschar_t = wchar_t;
#else
    using oschar_t = char;
#endif

    /// @}

} // namespace Geo

#endif // GEO_CORE_TYPES_H_INCLUDED
