# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

find_package("SDL2" REQUIRED)

add_executable("geo_client" WIN32
    "platform/desktop_sdl/client.cpp"
    "platform/desktop_sdl/display.cpp"
    "client.cpp"
    "main.cpp"
)

target_link_libraries("geo_client"
    PRIVATE
        "geo_compiler_options"
        "geo_math"
        "geo_system_gui"
        "geo_render"
        "SDL2::SDL2"
)
