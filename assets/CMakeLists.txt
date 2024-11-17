# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(PAK_BUILD_COMMAND "${VENV_PYTHON}" "${TOPDIR}/tools/mkzip.py" "-o" "${PAK_FILE}" "--min-comp-size=1024")
set(PAK_DEPENDENCIES "venv")

macro(pak_compress_files)
    foreach(FILE ${ARGV})
        list(APPEND PAK_BUILD_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/${FILE},${FILE},zstd,19")
        list(APPEND PAK_DEPENDENCIES "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
    endforeach()
endmacro()

#===================================================================================================
# Add files to the asset PAK
#===================================================================================================

pak_compress_files(
    "shaders/gl/color.frag"
    "shaders/gl/color.vert"
)

#===================================================================================================
# Build the asset PAK
#===================================================================================================

add_custom_command(
    OUTPUT "${PAK_FILE}"
    COMMAND ${PAK_BUILD_COMMAND}
    DEPENDS ${PAK_DEPENDENCIES})

add_custom_target("pak" ALL DEPENDS "${PAK_FILE}")
