# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION "3.25")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
project("geo" C CXX)

set(PAK_FILENAME "geo_data.pak")

set(BINDIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BINDIR}")

#===================================================================================================
# Compiler options

add_library("geo_compiler_options" INTERFACE)

target_compile_definitions("geo_compiler_options" INTERFACE
    $<$<CONFIG:MinSizeRel,RelWithDebInfo,Release>:NDEBUG>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options("geo_compiler_options" INTERFACE
        "-std=c++2a"
        "-pedantic"
        "-Wall"
        "-Wconversion"
        "-Wextra"
        "-Wsuggest-override"
    )
else()
    set(CMAKE_CXX_STANDARD "20")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions("geo_compiler_options" INTERFACE
        "_WIN32_WINNT=0x0400"
        "NOMINMAX"
    )
endif()

#===================================================================================================
# Set up Python virtual environment

find_package("Python3" REQUIRED)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(EXE_SUFFIX ".exe")
else()
    set(EXE_SUFFIX "")
endif()

set(VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/venv")
set(VENV_BINDIR "${VENV_DIR}/bin")
set(VENV_PYTHON "${VENV_BINDIR}/python${EXE_SUFFIX}")
set(VENV_PIP "${VENV_BINDIR}/pip${EXE_SUFFIX}")

add_custom_command(
    OUTPUT "${VENV_PYTHON}" "${VENV_PIP}"
    COMMAND "${Python3_EXECUTABLE}" "-m" "venv" "${VENV_DIR}"
    COMMAND "${VENV_PIP}" "install" "--upgrade" "pip"
    COMMAND "${VENV_PIP}" "install" "-r" "${SRCDIR}/tools/requirements.txt"
    DEPENDS "${SRCDIR}/tools/requirements.txt"
    USES_TERMINAL)

add_custom_target("venv" ALL DEPENDS "${VENV_PYTHON}")

#===================================================================================================
# Build

add_subdirectory("src")
add_subdirectory("assets")

#===================================================================================================
# Run

set(RUN_ARGS)

set(RUN_COMMAND "" CACHE STRING "Command prefix for running the game executable")

if(CMAKE_GENERATOR STREQUAL "Ninja" OR CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    add_custom_target("run"
        COMMAND ${RUN_COMMAND} "$<TARGET_FILE:geo_client>" ${RUN_ARGS}
        DEPENDS "geo_client" "pak"
        USES_TERMINAL)

    add_custom_target("run-verbose"
        COMMAND ${RUN_COMMAND} "$<TARGET_FILE:geo_client>" "-verbose" ${RUN_ARGS}
        DEPENDS "geo_client" "pak"
        USES_TERMINAL)
endif()
