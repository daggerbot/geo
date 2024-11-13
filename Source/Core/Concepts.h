/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_CONCEPTS_H_INCLUDED
#define GEO_CORE_CONCEPTS_H_INCLUDED

#include <concepts>
#include <type_traits>

#include "Types.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// Concept that matches any of the specified `Types`.
    template<typename T, typename...Types>
    concept AnyOf = (false || ... || std::same_as<T, Types>);

    /// Concept for fundamental character types. This does not include `signed char` or
    /// `unsigned char`, which are instead covered by @ref StrictInt, but this does include `char`,
    /// which is a always a distinct type from both `signed char` and `unsigned char` regardless of
    /// whether it is signed.
    template<typename T>
    concept CoreChar = AnyOf<T, char, wchar_t, char8_t, char16_t, char32_t>;

    /// Concept for fundamental integer types, excluding `bool`. This includes character types. In
    /// scenarios where a distinction between character and integer types is important, consider
    /// using @ref StrictInt instead.
    template<typename T>
    concept CoreInt = std::integral<T> && !std::same_as<T, bool>;

    /// Concept for fundamental integer types, excluding `bool` and anything matched by
    /// @ref CoreChar.
    template<typename T>
    concept StrictInt = CoreInt<T> && !CoreChar<T>;

    /// Concept for fundamental scalar types. This includes all fundamental integer types (including
    /// @ref CoreChar types, but not `bool`) and all fundamental floating point types.
    template<typename T>
    concept CoreScalar = CoreInt<T> || std::floating_point<T>;

    /// Concept for types that are contextually convertible to `bool`, allowing them to be used in
    /// conditional expressions, such as in `if` statements.
    template<typename T>
    concept Testable = requires(T t) {
        { t ? 1 : 0 };
    };

    /// Concept for types that can be compared for equality with `==` and `!=`.
    template<typename T, typename R = T>
    concept Equatable = requires(T t, R r) {
        { t == r } -> Testable;
        { t != r } -> Testable;
    };

    /// Concept for types that can be compared for ordering with `==`, `!=`, `<`, `<=`, `>`, and `>=`.
    template<typename T, typename R = T>
    concept PartiallyOrdered =
        Equatable<T, R>
        && requires(T t, R r) {
            { t < r } -> Testable;
            { t <= r } -> Testable;
            { t > r } -> Testable;
            { t >= r } -> Testable;
        };

    /// @}

} // namespace Geo

#endif // GEO_CORE_CONCEPTS_H_INCLUDED
