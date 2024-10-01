# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

#===================================================================================================
# geo_system

add_library("geo_system" STATIC
    "system.cpp"
)

target_compile_definitions("geo_system" PUBLIC "LOG_PATH_PREFIX=\"${LOG_PATH_PREFIX}\"")
target_include_directories("geo_system" PUBLIC "include")

target_link_libraries("geo_system"
    PUBLIC
        "geo_core"
    PRIVATE
        "geo_compiler_options"
)

if(UNIX)
    target_sources("geo_system" PRIVATE
        "platform/unix/debug.cpp"
        "platform/unix/system.cpp"
    )
endif()

#===================================================================================================
# geo_system_gui

add_library("geo_system_gui" STATIC "system_no_gui.cpp")

target_link_libraries("geo_system_gui"
    PUBLIC
        "geo_system"
    PRIVATE
        "geo_compiler_options"
)
