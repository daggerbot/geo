# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(GL_VERSION "4.5")

#===================================================================================================
# geo_common: Code shared between the client and dedicated server
#===================================================================================================

find_package("fmt" REQUIRED)
find_package("libzip" REQUIRED)

add_library("geo_common" STATIC
    "io/error.cpp"
    "io/stream.cpp"
    "io/zip.cpp"
    "system/cmdline.cpp"
    "system/debug.cpp"
    "system/error.cpp"
    "system/system.cpp"
)

target_include_directories("geo_common" PUBLIC ".")

target_link_libraries("geo_common"
    PUBLIC
        "fmt::fmt"
    PRIVATE
        "geo_compiler_options"
        "libzip::zip"
)

if(UNIX)
    target_sources("geo_common" PRIVATE
        "system/unix/debug.cpp"
        "system/unix/system.cpp"
    )
endif()

#===================================================================================================
# geo_client
#===================================================================================================

find_package("SDL2" REQUIRED)

add_executable("geo_client" WIN32
    "client/display.cpp"
    "client/main.cpp"
    "client/playground.cpp"
    "render/gl/buffer.cpp"
    "render/gl/geometry.cpp"
    "render/gl/render.cpp"
    "render/gl/shaders.cpp"
    "render/gl/utils.cpp"
)

target_link_libraries("geo_client" PRIVATE
    "geo_compiler_options"
    "geo_common"
    "SDL2::SDL2"
)

#===================================================================================================
# Generate <core/game_defs.h>
#===================================================================================================

set(GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")

string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)" "\\1" GL_MAJOR_VERSION "${GL_VERSION}")
string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)" "\\2" GL_MINOR_VERSION "${GL_VERSION}")

configure_file("core/game_defs.h.in" "${GENERATED_INCLUDE_DIR}/core/game_defs.h")
target_include_directories("geo_common" PUBLIC "${GENERATED_INCLUDE_DIR}")

#===================================================================================================
# glad (OpenGL API loader)
#===================================================================================================

set(VENV_GLAD "${VENV_BINDIR}/glad${HOST_EXE_SUFFIX}")
set(GLAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/glad")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")
set(GLAD_SOURCE "${GLAD_DIR}/src/gl.c")
set(GLAD_HEADER "${GLAD_INCLUDE_DIR}/glad/gl.h")

add_custom_command(
    OUTPUT "${GLAD_SOURCE}" "${GLAD_HEADER}"
    COMMAND "${VENV_GLAD}" "--api=gl:core=${GL_VERSION}"
                           "--extensions="
                           "--out-path=${GLAD_DIR}"
                           "--quiet"
                           "--reproducible"
    DEPENDS "venv")

add_library("glad" STATIC "${GLAD_SOURCE}")
target_include_directories("glad" PUBLIC "${GLAD_INCLUDE_DIR}")
target_link_libraries("geo_client" PRIVATE "glad")
