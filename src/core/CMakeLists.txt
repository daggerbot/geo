# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library("geo_core" STATIC
    "format.cpp"
    "parse.cpp"
    "types.cpp"
)

target_include_directories("geo_core" PUBLIC "include")

target_link_libraries("geo_core"
    PRIVATE
        "geo_compiler_options"
)
