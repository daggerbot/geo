/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>
#include <unistd.h>

#include <core/game_defs.h>
#include <system/debug.h>
#include <system/system.h>

using namespace geo;

OsString system::get_default_pak_path()
{
    LOG_WARNING("Specifying the assets path with --assets=PATH is recommended on this platform");

    // Get the search path.
    const char* dirs = getenv("XDG_DATA_DIRS");

    if (!dirs || !dirs[0])
        dirs = "/usr/local/share:/usr/share";

    LOG_INFO("Searching for '{}' in: {}", PAK_FILENAME, dirs);

    // For each directory, try `<dir>/games/<game-id>/<pak-filename>`
    std::string path;
    const char* pos = dirs;
    const char* end = dirs;

    for (;;) {
        if (*end == 0 || *end == ':') {
            if (end != pos) {
                path = {pos, end};
                path += "/games/" GAME_ID "/" PAK_FILENAME;
                LOG_TRACE("Checking path: {}", path);

                if (access(path.c_str(), R_OK) == 0)
                    return path;
            }

            if (*end == 0)
                FATAL("Can't find '{}' in search path: {}", PAK_FILENAME, dirs);

            pos = ++end;
        } else {
            ++end;
        }
    }
}
