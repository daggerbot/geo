/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_FINALLY_H_INCLUDED
#define GEO_CORE_FINALLY_H_INCLUDED

#include <type_traits>

#include "Types.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// Object that invokes a callback when destroyed. This is typically used to defer resource
    /// cleanup to the end of a scope.
    template<std::invocable T>
    class Finally {
    public:
        constexpr Finally()
        requires (std::is_default_constructible_v<T>)
            : m_callback{}
        {
        }

        Finally(const Finally<T>&) = delete;

        constexpr Finally(T&& callback)
        requires (std::is_move_constructible_v<T>)
            : m_callback{std::move(callback)}
        {
        }

        constexpr ~Finally()
        {
            m_callback();
        }

        Finally<T>& operator=(const Finally<T>&) = delete;

    private:
        T m_callback;
    };

    /// @}

} // namespace Geo

#endif // GEO_CORE_FINALLY_H_INCLUDED
