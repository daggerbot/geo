/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_ASCII_H_INCLUDED
#define GEO_CORE_ASCII_H_INCLUDED

#include "Concepts.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// Utilities for handling ASCII-encoded text.
    namespace Ascii {

        template<CoreInt CharT>
        constexpr bool IsDigit(CharT ch)
        {
            return ch >= '0' && ch <= '9';
        }

        template<CoreInt CharT>
        constexpr bool IsUppercase(CharT ch)
        {
            return ch >= 'A' && ch <= 'Z';
        }

        template<CoreInt CharT>
        constexpr bool IsLowercase(CharT ch)
        {
            return ch >= 'a' && ch <= 'z';
        }

        template<CoreInt CharT>
        constexpr bool IsAlphabetic(CharT ch)
        {
            return IsUppercase(ch) || IsLowercase(ch);
        }

        template<CoreInt CharT>
        constexpr bool IsAlphanumeric(CharT ch)
        {
            return IsDigit(ch) || IsAlphabetic(ch);
        }

        /// Returns the numeric value of an alphanumeric ASCII character (0-9, A-Z, a-z) for bases
        /// up to 36. Returns -1 if the character is not alphanumeric.
        template<CoreInt CharT>
        constexpr int GetAlphanumericDigitValue(CharT ch)
        {
            if (IsDigit(ch))
                return int(ch - '0');
            else if (IsUppercase(ch))
                return int(ch - 'A' + 10);
            else if (IsLowercase(ch))
                return int(ch - 'a' + 10);
            else
                return -1;
        }

    } // namespace Ascii

    /// @}

} // namespace Geo

#endif // GEO_CORE_ASCII_H_INCLUDED
