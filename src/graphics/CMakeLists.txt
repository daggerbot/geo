# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library("geo_graphics" INTERFACE)
target_include_directories("geo_graphics" INTERFACE "include")
target_link_libraries("geo_graphics" INTERFACE "geo_math")
