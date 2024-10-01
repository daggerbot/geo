/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_ASCII_H_INCLUDED
#define GEO_CORE_ASCII_H_INCLUDED

#include "concepts.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Utilities for handling ASCII-encoded text.
    namespace ascii {

        template<CoreInt CharT>
        constexpr bool is_digit(CharT ch)
        {
            return ch >= '0' && ch <= '9';
        }

        template<CoreInt CharT>
        constexpr bool is_uppercase(CharT ch)
        {
            return ch >= 'A' && ch <= 'Z';
        }

        template<CoreInt CharT>
        constexpr bool is_lowercase(CharT ch)
        {
            return ch >= 'a' && ch <= 'z';
        }

        template<CoreInt CharT>
        constexpr bool is_alphabetic(CharT ch)
        {
            return ascii::is_uppercase(ch) || ascii::is_lowercase(ch);
        }

        template<CoreInt CharT>
        constexpr bool is_alphanumeric(CharT ch)
        {
            return ascii::is_digit(ch) || ascii::is_alphabetic(ch);
        }

    } // namespace ascii

    /// @}

} // namespace geo

#endif // GEO_CORE_ASCII_H_INCLUDED
