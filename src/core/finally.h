/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CORE_FINALLY_H_INCLUDED
#define CORE_FINALLY_H_INCLUDED

#include <concepts>
#include <type_traits>

#include "types.h"

namespace geo {

    /// Object that invokes a callback when destroyed. This is typically used to clean up resources
    /// when the end of a scope is reached.
    template<std::invocable T>
    class Finally {
    public:
        constexpr Finally()
        requires (std::is_default_constructible_v<T>)
            : callback_{}
        {
        }

        Finally(const Finally<T>&) = delete;

        constexpr Finally(T&& callback)
        requires (std::is_move_constructible_v<T>)
            : callback_{std::move(callback)}
        {
        }

        constexpr ~Finally()
        {
            callback_();
        }

        Finally<T>& operator=(const Finally<T>&) = delete;

    private:
        T callback_;
    };

} // namespace geo

#endif // CORE_FINALLY_H_INCLUDED
