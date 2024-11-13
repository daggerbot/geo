# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(GL_VERSION "4.3")

#===================================================================================================
# GeoCommon: Code shared between client and server
#===================================================================================================

find_package("libzip" REQUIRED)

add_library("GeoCommon" STATIC
    "Core/FormatTests.cpp"
    "Core/ParseTests.cpp"
    "Core/TypeTests.cpp"
    "IO/ArchiveCodecs/Zip.cpp"
    "IO/Error.cpp"
    "IO/Stream.cpp"
    "System/Debug.cpp"
    "System/Error.cpp"
    "System/System.cpp"
    "Utility/CommandLine.cpp"
)

target_include_directories("GeoCommon" PUBLIC ".")

target_link_libraries("GeoCommon"
    PRIVATE
        "GeoCompilerOptions"
        "libzip::zip"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources("GeoCommon" PRIVATE
        "System/Windows/Debug.cpp"
        "System/Windows/Encoding.cpp"
        "System/Windows/System.cpp"
        "System/Windows/Win32.cpp"
    )
elseif(UNIX)
    target_sources("GeoCommon" PRIVATE
        "System/Unix/Debug.cpp"
        "System/Unix/System.cpp"
    )
endif()

#===================================================================================================
# GeoClient
#===================================================================================================

find_package("SDL2" REQUIRED)

add_executable("GeoClient" WIN32
    "Client/Sdl/Display.cpp"
    "Client/Sdl/Main.cpp"
    "Client/Main.cpp"
    "Client/Playground.cpp"
    "Render/GL/Buffer.cpp"
    "Render/GL/Geometry.cpp"
    "Render/GL/Gl.cpp"
    "Render/GL/Shaders.cpp"
    "Render/GL/System.cpp"
)

target_link_libraries("GeoClient" PRIVATE
    "GeoCompilerOptions"
    "GeoCommon"
    "SDL2::SDL2"
)

#===================================================================================================
# Generate <Core/GameDefs.h>
#===================================================================================================

set(GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")

string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)" "\\1" GL_MAJOR_VERSION "${GL_VERSION}")
string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)" "\\2" GL_MINOR_VERSION "${GL_VERSION}")

configure_file("Core/GameDefs.h.in" "${GENERATED_INCLUDE_DIR}/Core/GameDefs.h")
target_include_directories("GeoCommon" PUBLIC "${GENERATED_INCLUDE_DIR}")

#===================================================================================================
# glad (OpenGL API loader)
#===================================================================================================

set(VENV_GLAD "${VENV_BINDIR}/glad${EXE_SUFFIX}")
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
target_include_directories("glad" SYSTEM PUBLIC "${GLAD_INCLUDE_DIR}")

target_link_libraries("GeoClient" PRIVATE "glad")
