/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_SYSTEM_SYSTEM_H_INCLUDED
#define GEO_SYSTEM_SYSTEM_H_INCLUDED

#include <memory>

#include <Core/StringTypes.h>
#include <IO/Stream.h>

namespace Geo {

    class StreamProvider;

    /// @addtogroup System
    /// @{

    /// Core utilities and platform abstractions essential for initializing and managing the game's
    /// runtime environment.
    namespace System {

        /// Gets the platform's default path for the game's asset PAK.
        OsString GetDefaultAssetPakPath();

        /// Opens the game's asset PAK. If `path` is null, then @ref GetDefaultAssetPakPath is used.
        std::unique_ptr<StreamProvider> OpenAssetPak(const oschar_t* path);

    } // namespace System

    /// @}

} // namespace Geo

#endif // GEO_SYSTEM_SYSTEM_H_INCLUDED
