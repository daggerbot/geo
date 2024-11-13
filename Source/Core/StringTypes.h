/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_STRINGTYPES_H_INCLUDED
#define GEO_CORE_STRINGTYPES_H_INCLUDED

#include <string>

#include "Types.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    using OsString = std::basic_string<oschar_t>;
    using OsStringView = std::basic_string_view<oschar_t>;

    /// @}

} // namespace Geo

#endif // GEO_CORE_STRINGTYPES_H_INCLUDED
