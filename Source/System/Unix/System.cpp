/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>
#include <unistd.h>

#include <Core/GameDefs.h>
#include <System/Debug.h>
#include <System/System.h>

using namespace Geo;

OsString System::GetDefaultAssetPakPath()
{
    LOG_WARNING("Specifying the asset PAK path with --pak=PATH is recommended on this platform");

    // Search XDG_DATA_DIRS or a fallback.
    const char* dirs = getenv("XDG_DATA_DIRS");

    if (!dirs || !dirs[0])
        dirs = "/usr/local/share:/usr/share";

    LOG_INFO("Searching for '" PAK_FILENAME "' in: {}", dirs);

    // For each directory, try '<dir>/games/<game-id>/<pak-filename>'.
    // Loop through entries in `dirs`, separated by `:`.
    std::string path;
    const char* pos = dirs;
    const char* end = dirs;

    for (;;) {
        if (*end == 0 || *end == ':') {
            if (end != pos) {
                path = {pos, end};
                path += "/games/" GAME_ID "/" PAK_FILENAME;
                LOG_TRACE("Checking path: {}", path);

                // Check if we have read access to the file at `path`.
                if (access(path.c_str(), R_OK) == 0)
                    return path;
            }

            if (*end == 0)
                FATAL("Can't find '" PAK_FILENAME "' in search path: {}", dirs);

            pos = end + 1;
            end = pos;
        } else {
            ++end;
        }
    }
}
