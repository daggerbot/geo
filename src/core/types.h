/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CORE_TYPES_H_INCLUDED
#define CORE_TYPES_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

/// @def OSSTR
/// Encodes the following string literal as an array of @ref geo::oschar_t.
#ifdef _WIN32
# define OSSTR L""
#else
# define OSSTR
#endif

namespace geo {

    using nullptr_t = std::nullptr_t;
    using ptrdiff_t = std::ptrdiff_t;
    using size_t = std::size_t;
    using ssize_t = std::make_signed_t<size_t>;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
    using iptr = std::intptr_t;

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using uptr = std::uintptr_t;

    using f32 = float;
    using f64 = double;

    /// @typedef oschar_t
    /// Preferred character type when using the system APIs.
    /// This is typically `char`, with the notable exception that it is `wchar_t` on Windows.
#ifdef _WIN32
    using oschar_t = wchar_t;
#else
    using oschar_t = char;
#endif

    using OsString = std::basic_string<oschar_t>;
    using OsStringView = std::basic_string_view<oschar_t>;

} // namespace geo

#endif // CORE_TYPES_H_INCLUDED
