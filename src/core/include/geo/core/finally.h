/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_FINALLY_H_INCLUDED
#define GEO_CORE_FINALLY_H_INCLUDED

#include <concepts>

#include "types.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Object that invokes a callback when destroyed. This is typically used to clean up resources
    /// when the end of a scope is reached.
    template<std::invocable T>
    class Finally {
    public:
        /// Default-constructs the callback.
        constexpr Finally()
        requires std::is_default_constructible_v<T>
            : m_callback{}
        {
        }

        Finally(const Finally<T>&) = delete;

        /// Move-constructs the fallback.
        constexpr Finally(T&& callback)
        requires std::is_move_constructible_v<T>
            : m_callback{std::move(callback)}
        {
        }

        /// Invokes the callback if it has not been inhibited.
        constexpr ~Finally()
        {
            if (!m_inhibit)
                m_callback();
        }

        Finally<T>& operator=(const Finally<T>&) = delete;

        /// Prevents the callback from being called.
        constexpr void inhibit() { m_inhibit = true; }

    private:
        T m_callback;
        bool m_inhibit = false;
    };

    /// @}

} // namespace geo

#endif // GEO_CORE_FINALLY_H_INCLUDED
