/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <geo/core/format.h>
#include <geo/core/game_defs.h>
#include <geo/system/system.h>

using namespace geo;

/*
std::vector<OsString> system::get_data_dirs()
{
    std::vector<std::string> dirs;
    const char* xdg_config_home = ::getenv("XDG_CONFIG_HOME");

    if (!xdg_config_home || !xdg_config_home[0])
        xdg_config_home = "/usr/local/share:/usr/share";

    const char* dirStart = xdg_config_home;
    const char* dirEnd = xdg_config_home;

    for (;;) {
        if (*dirEnd == 0 || *dirEnd == ':') {
            if (dirEnd != dirStart)
                dirs.push_back(format::string("{}/games/" GAME_ID, std::string_view{dirStart, size_t(dirEnd - dirStart)}));

            if (*dirEnd == 0) {
                if (dirs.empty())
                    FATAL("No valid directories in XDG_CONFIG_HOME");

                return dirs;
            }

            dirStart = ++dirEnd;
        } else {
            ++dirEnd;
        }
    }
}
*/

void system::exit_with_error_message(const oschar_t* message)
{
    if (message) {
        ::fprintf(stderr, "FATAL ERROR: %s\n", message);
        ::fflush(stderr);
        system::show_error_dialog(message);
    }

    ::exit(EXIT_FAILURE);
}

std::string system::strerror_a(int errnum)
{
    char buf[512] = {0};

    ::strerror_r(errnum, buf, sizeof(buf));

    if (!buf[0])
        ::snprintf(buf, sizeof(buf), "errno %d", errnum);

    return buf;
}
