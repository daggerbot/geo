# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

#===================================================================================================
# Generate <geo/render/gl/version.h>

set(GL_VERSION "3.2")

string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)" "\\1" GL_VERSION_MAJOR "${GL_VERSION}")
string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)" "\\2" GL_VERSION_MINOR "${GL_VERSION}")
set(GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
set(GL_VERSION_H "${GENERATED_INCLUDE_DIR}/geo/render/gl/version.h")
configure_file("gl/version.h.in" "${GL_VERSION_H}" @ONLY)

#===================================================================================================
# Build glad (OpenGL loader)

set(VENV_GLAD "${VENV_BINDIR}/glad${EXE_SUFFIX}")
set(GLAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/glad")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")
set(GLAD_SOURCE "${GLAD_DIR}/src/gl.c")
set(GLAD_HEADER "${GLAD_INCLUDE_DIR}/glad/gl.h")

add_custom_command(
    OUTPUT "${GLAD_SOURCE}" "${GLAD_HEADER}"
    COMMAND "${VENV_GLAD}" "--api" "gl:core=${GL_VERSION}"
                           "--extensions" "${CMAKE_CURRENT_SOURCE_DIR}/gl/extensions.txt"
                           "--out-path" "${GLAD_DIR}"
                           "--reproducible"
    DEPENDS "venv"
    USES_TERMINAL)

add_library("glad" STATIC "${GLAD_SOURCE}")
target_include_directories("glad" PUBLIC "${GLAD_INCLUDE_DIR}")

#===================================================================================================
# Build geo_render

add_library("geo_render" STATIC
    "gl/render_system.cpp"
)

target_include_directories("geo_render" PUBLIC "include" "${GENERATED_INCLUDE_DIR}")

target_link_libraries("geo_render"
    PUBLIC
        "geo_system"
        "geo_graphics"
    PRIVATE
        "geo_compiler_options"
        "glad"
)
