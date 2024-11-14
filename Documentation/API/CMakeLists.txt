# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

find_package("Doxygen")

if(DOXYGEN_FOUND)
    configure_file("Doxyfile.in" "Doxyfile")
    add_custom_target("api-docs" COMMAND "${DOXYGEN_EXECUTABLE}")
else()
    message(WARNING "Doxygen not found; can't generate API documentation")
endif()
