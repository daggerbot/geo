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

#include "types.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Concept that matches any of the specified `Types`.
    template<typename T, typename...Types>
    concept AnyOf = (false || ... || std::same_as<T, Types>);

    /// Concept for the fundamental character types.
    template<typename T>
    concept CoreChar = AnyOf<T, char, wchar_t, char8_t, char16_t, char32_t>;

    /// Concept for the fundamental integer types, excluding `bool`.
    template<typename T>
    concept CoreInt = std::integral<T> && !std::same_as<T, bool>;

    /// Concept for the fundamental integer types, excluding `bool` and the @ref CoreChar types.
    template<typename T>
    concept StrictInt = CoreInt<T> && !CoreChar<T>;

    /// Concept for types that are contextually convertible to `bool`, allowing them to be used in
    /// conditional expressions, i.e. `if` statements.
    template<typename T>
    concept Testable = requires(T t) {
        { t ? 1 : 0 };
    };

    /// Concept for types that can be compared for equality with `==` and `!=`.
    template<typename T, typename R = T>
    concept Equatable = requires(T lhs, R rhs) {
        { lhs == rhs } -> Testable;
        { lhs != rhs } -> Testable;
    };

    /// Concept for types that can be compared for ordering with `==`, `!=`, `<`, `<=`, `>`, and
    /// `>=`.
    template<typename T, typename R = T>
    concept PartiallyOrdered =
        Equatable<T, R>
        && requires(T lhs, T rhs) {
            { lhs < rhs } -> Testable;
            { lhs <= rhs } -> Testable;
            { lhs > rhs } -> Testable;
            { lhs >= rhs } -> Testable;
        };

    /// @}

} // namespace geo

#endif // GEO_CORE_CONCEPTS_H_INCLUDED
