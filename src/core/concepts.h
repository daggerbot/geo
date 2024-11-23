/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CORE_CONCEPTS_H_INCLUDED
#define CORE_CONCEPTS_H_INCLUDED

#include <concepts>

#include "types.h"

namespace geo {

    /// Concept for fundamental integral types, excluding `bool`.
    template<typename T>
    concept CoreInt = std::integral<T> && !std::same_as<T, bool>;

    /// Concept for types that are contextually convertible to `bool`, enabling their use in
    /// conditional expressions (e.g., `if` and `while` statements).
    template<typename T>
    concept Testable = requires(T t) {
        { t ? 0 : 0 };
    };

    /// Concept for types that support equality comparison.
    template<typename T, typename R = T>
    concept Equatable = requires(T t, R r) {
        { t == r } -> Testable;
        { t != r } -> Testable;
    };

    /// Concept for types that support relational comparison (e.g., `operator<`, `operator>`, etc.).
    template<typename T, typename R = T>
    concept Ordered =
        Equatable<T, R>
        && requires(T t, R r) {
            { t < r } -> Testable;
            { t <= r } -> Testable;
            { t > r } -> Testable;
            { t >= r } -> Testable;
        };

} // namespace geo

#endif // CORE_CONCEPTS_H_INCLUDED
