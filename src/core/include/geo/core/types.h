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
#include <type_traits>
#include <utility>

#include "compiler_defs.h"

/// @addtogroup geo_core
/// @{

/// @def OSCHAR_WIDE
/// 1 if @ref geo::oschar_t is `wchar_t`, or 0 if it is `char`.
#ifdef _WIN32
# define OSCHAR_WIDE 1
#else
# define OSCHAR_WIDE 0
#endif

/// @def OSSTR
/// Encodes the following string literal as an array of @ref geo::oschar_t.
#if OSCHAR_WIDE
# define OSSTR L""
#else
# define OSSTR
#endif

/// @}

namespace geo {

    /// @addtogroup geo_core
    /// @{

    using max_align_t = std::max_align_t;
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
    /// Preferred character type when using the system APIs. This is typically `char`, with the
    /// notable exception that it is `wchar_t` on Windows.
#if OSCHAR_WIDE
    using oschar_t = wchar_t;
#else
    using oschar_t = char;
#endif

    /// Reference wrapper for explicitly marking function parameters as output values.
    template<typename T>
    requires (!std::is_const_v<T> && !std::is_reference_v<T> && !std::is_same_v<T, void>)
    class out {
    public:
        out() = delete;

        /// Copies an `out` reference.
        constexpr out(const out<T>&) = default;

        /// Copies an `out` reference with a compatible target type.
        template<typename U>
        constexpr out(const out<U>& other)
        requires std::is_convertible_v<U&, T&>
            : m_ref{other.as_ref()}
        {
        }

        /// Constructs an `out` from a reference.
        constexpr out(T& ref)
            : m_ref{ref}
        {
        }

        out<T>& operator=(const out<T>&) = delete;

        /// Dereferences the target object.
        constexpr T& operator*() const { return m_ref; }

        /// Dereferences the target object.
        constexpr T* operator->() const { return &m_ref; }

        /// Returns a pointer to the target object.
        constexpr T* as_ptr() const { return &m_ref; }

        /// Returns a reference to the target object.
        constexpr T& as_ref() const { return m_ref; }

    private:
        T& m_ref;
    };

    /// @}

} // namespace geo

#endif // GEO_CORE_TYPES_H_INCLUDED
