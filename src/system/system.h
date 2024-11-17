/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_SYSTEM_H_INCLUDED
#define SYSTEM_SYSTEM_H_INCLUDED

#include <memory>
#include <span>

#include <core/strings.h>

namespace geo {

    class StreamProvider;

    /// Functions for interacting with the operating system.
    namespace system {

        /// Gets the default path to the asset PAK. Used if not specified from the command line.
        OsString get_default_pak_path();

        /// Opens the asset PAK. If `path` is null, @ref get_default_pak_path is used.
        std::unique_ptr<StreamProvider> open_pak(const OsChar* path);

    } // namespace system

} // namespace geo

#endif // SYSTEM_SYSTEM_H_INCLUDED
