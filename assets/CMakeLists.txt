# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(PAK_FILE "${BINDIR}/${PAK_FILENAME}")
set(MIN_COMPRESS_SIZE "4096")
set(ZSTD_LEVEL "19")

set(PAK_BUILD_COMMAND "${VENV_PYTHON}" "${SRCDIR}/tools/mkzip.py" "-o" "${PAK_FILE}" "--min-comp-size" "${MIN_COMPRESS_SIZE}")
set(PAK_DEPENDENCIES "venv")

#===================================================================================================
# Assets with no additional build step

set(RAW_FILES
    "shaders/glsl150/color.frag"
    "shaders/glsl150/color.vert"
    "shaders/color_2d.t"
)

foreach(FILE ${RAW_FILES})
    list(APPEND PAK_BUILD_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/${FILE},${FILE},zstd,${ZSTD_LEVEL}")
    list(APPEND PAK_DEPENDENCIES "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
endforeach()

#===================================================================================================
# Build the asset pack

add_custom_command(
    OUTPUT "${PAK_FILE}"
    COMMAND ${PAK_BUILD_COMMAND}
    DEPENDS ${PAK_DEPENDENCIES})

add_custom_target("pak" ALL DEPENDS "${PAK_FILE}")
