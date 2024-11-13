# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION "3.25")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules")
project("Geo" C CXX)

set(TOPDIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BINDIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BINDIR}")

set(PAK_FILENAME "GeoData.pak")
set(PAK_FILE "${BINDIR}/${PAK_FILENAME}")

#===================================================================================================
# Compiler options
#===================================================================================================

add_library("GeoCompilerOptions" INTERFACE)

target_compile_definitions("GeoCompilerOptions" INTERFACE
    $<$<CONFIG:Debug>:LOG_LEVEL_DEFAULT=LOG_LEVEL_DEBUG>
    $<$<CONFIG:MinSizeRel>:LOG_LEVEL_MAX=LOG_LEVEL_FATAL>
    $<$<CONFIG:Debug>:LOG_LEVEL_MAX=LOG_LEVEL_TRACE>
    $<$<CONFIG:RelWithDebInfo,Release,MinSizeRel>:NDEBUG>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options("GeoCompilerOptions" INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
        "-std=c++2a"
        "-pedantic"
        "-fmacro-prefix-map=${TOPDIR}/Source/="
        "-fno-exceptions"
        "-fno-rtti"
        "-Wall"
        "-Wconversion"
        "-Wextra"
        "-Wsuggest-override"
    >)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions("GeoCompilerOptions" INTERFACE
        "_WIN32_WINNT=0x0400"
        "NOMINMAX"
    )
endif()

#===================================================================================================
# Set up Python virtual environment (venv)
#===================================================================================================

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
    COMMAND "${VENV_PIP}" "install" "-U" "pip"
    COMMAND "${VENV_PIP}" "install" "-r" "${TOPDIR}/Tools/Requirements.txt"
    DEPENDS "${TOPDIR}/Tools/Requirements.txt"
    USES_TERMINAL)

add_custom_target("venv" ALL DEPENDS "${VENV_PYTHON}" "${VENV_PIP}")
set_property(TARGET "venv" APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${VENV_DIR}")

#===================================================================================================
# Build components
#===================================================================================================

add_subdirectory("Source")
add_subdirectory("Assets")

#===================================================================================================
# Test run
#===================================================================================================

set(RUN_ARGS
    "--pak=${PAK_FILE}"
)

set(RUN_COMMAND "" CACHE STRING "Command for running the game executable")
set(DEBUG_COMMAND "gdb" "--args" CACHE STRING "Command for debugging the game executable")

if(CMAKE_GENERATOR STREQUAL "Ninja" OR CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    add_custom_target("run"
        COMMAND ${RUN_COMMAND} "$<TARGET_FILE:GeoClient>" ${RUN_ARGS}
        DEPENDS "GeoClient" "pak"
        USES_TERMINAL)

    add_custom_target("run-trace"
        COMMAND ${RUN_COMMAND} "$<TARGET_FILE:GeoClient>" "--log-level=trace" ${RUN_ARGS}
        DEPENDS "GeoClient" "pak"
        USES_TERMINAL)

    add_custom_target("debug"
        COMMAND ${DEBUG_COMMAND} "$<TARGET_FILE:GeoClient>" ${RUN_ARGS}
        DEPENDS "GeoClient" "pak"
        USES_TERMINAL)

    add_custom_target("debug-trace"
        COMMAND ${DEBUG_COMMAND} "$<TARGET_FILE:GeoClient>" "--log-level=trace" ${RUN_ARGS}
        DEPENDS "GeoClient" "pak"
        USES_TERMINAL)
endif()
