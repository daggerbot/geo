/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifdef _WIN32
# include <windows.h>
#endif

#include <geo/core/string_utils.h>
#include <geo/render/system.h>
#include <geo/system/debug.h>

#include "client.h"
#include "display.h"
#include "playground.h"

using namespace geo;

namespace {

    void handle_command_line(int argc, const oschar_t* const argv[])
    {
        for (int i = 1; i < argc; ++i) {
            if (string_utils::equals(argv[i], OSSTR "-verbose")) {
                debug::enable_verbose();
            } else if (argv[i][0] == '-') {
                FATAL("Invalid option: {}", argv[i]);
            } else {
                FATAL("Unexpected operand: {}", argv[i]);
            }
        }
    }

    /// Cross-platform entry point.
    int geo_main(int argc, const oschar_t* const argv[])
    {
        Client client;

        ::handle_command_line(argc, argv);

        LOG_INFO("Initializing...");
        client.initialize();
        client.set_state(std::make_unique<Playground>());

        LOG_INFO("Game started!");
        client.main_loop();

        LOG_INFO("Shutting down...");
        client.shut_down();

        return 0;
    }

} // namespace

#ifdef _WIN32

/// Windows entry point.
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    return ::geo_main(__argc, __wargv);
}

#else // !defined(_WIN32)

/// ISO C++ entry point.
int main(int argc, char* argv[])
{
    return ::geo_main(argc, argv);
}

#endif // !defined(_WIN32)
