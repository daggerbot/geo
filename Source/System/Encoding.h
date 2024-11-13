/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_SYSTEM_ENCODING_H_INCLUDED
#define GEO_SYSTEM_ENCODING_H_INCLUDED

#include <Core/Format.h>

namespace Geo {

    /// @addtogroup System
    /// @{

    /// Functions for converting text.
    namespace Encoding {

        /// Converts a string from the system narrow encoding to the system wide encoding.
        std::wstring SystemNarrowToWide(std::string_view str);

    } // namespace Encoding

    template<>
    struct Formatter<std::string_view, wchar_t> : Formatter<std::wstring, wchar_t> {
        void Format(std::string_view str, FormatWriter<wchar_t>& writer) const
        {
            Formatter<std::wstring, wchar_t>::Format(Encoding::SystemNarrowToWide(str), writer);
        }
    };

    /// @}

} // namespace Geo

#endif // GEO_SYSTEM_ENCODING_H_INCLUDED
