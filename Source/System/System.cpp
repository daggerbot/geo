/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <IO/ArchiveCodecs/Zip.h>

#include "Debug.h"
#include "System.h"

using namespace Geo;

std::unique_ptr<StreamProvider> System::OpenAssetPak(const oschar_t* explicitPath)
{
    auto archive = std::make_unique<ZipArchive>();
    OsString path;
    Error error;

    if (explicitPath)
        path = explicitPath;
    else
        path = GetDefaultAssetPakPath();

    LOG_INFO("Reading assets from: {}", path);

    if (!archive->Open(path.c_str(), error))
        FATAL("{}: {}", path, error);

    return archive;
}
