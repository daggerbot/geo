/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <io/zip.h>

#include "debug.h"
#include "system.h"

using namespace geo;

std::unique_ptr<StreamProvider> system::open_pak(const oschar_t* explicit_path)
{
    OsString path;

    if (explicit_path)
        path = explicit_path;
    else
        path = get_default_pak_path();

    LOG_INFO("Reading assets from: {}", path);

    Error error;
    auto archive = std::make_unique<ZipArchive>(path.c_str(), error);

    if (error)
        FATAL("{}: {}", path, error);

    return archive;
}
