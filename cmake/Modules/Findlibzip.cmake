# Copyright (c) 2024 Marty Mills <daggerbot@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

cmake_minimum_required(VERSION "3.19")

if(NOT TARGET "libzip::zip")
    include("FindPackageHandleStandardArgs")

    unset(libzip_VERSION)

    find_library(libzip_LIBRARY "zip"
        HINTS "$ENV{ZIPDIR}"
        PATHS "${libzip_PREFIX}")

    find_path(libzip_INCLUDE_DIR "zip.h"
        HINTS "$ENV{ZIPDIR}"
        PATHS "${libzip_PREFIX}"
        PATH_SUFFIXES "include")

    if(libzip_LIBRARY AND libzip_INCLUDE_DIR)
        file(READ "${libzip_INCLUDE_DIR}/zipconf.h" _ZIPCONF_H)
        string(REGEX MATCH "#define LIBZIP_VERSION \"[0-9]+\\.[0-9]+\\.[0-9]+\"" _libzip_VERSION "${_ZIPCONF_H}")

        if(_libzip_VERSION)
            string(REGEX REPLACE "#define LIBZIP_VERSION \"([0-9])+\\.([0-9])+\\.([0-9]+)\"" "\\1" libzip_VERSION_MAJOR "${_libzip_VERSION}")
            string(REGEX REPLACE "#define LIBZIP_VERSION \"([0-9])+\\.([0-9])+\\.([0-9]+)\"" "\\2" libzip_VERSION_MINOR "${_libzip_VERSION}")
            string(REGEX REPLACE "#define LIBZIP_VERSION \"([0-9])+\\.([0-9])+\\.([0-9]+)\"" "\\3" libzip_VERSION_PATCH "${_libzip_VERSION}")
            set(libzip_VERSION "${libzip_VERSION_MAJOR}.${libzip_VERSION_MINOR}.${libzip_VERSION_PATCH}" CACHE INTERNAL "")
        endif()

        unset(_ZIPCONF_H)
        unset(_libzip_VERSION)
    endif()

    if(libzip_VERSION)
        set(libzip_FOUND ON CACHE INTERNAL "")
        add_library("libzip::zip" INTERFACE IMPORTED GLOBAL)
        target_include_directories("libzip::zip" SYSTEM INTERFACE "${libzip_INCLUDE_DIR}")
        target_link_libraries("libzip::zip" INTERFACE "${libzip_LIBRARY}")
    endif()

    find_package_handle_standard_args("libzip"
        REQUIRED_VARS libzip_LIBRARY libzip_FOUND
        VERSION_VAR libzip_VERSION
        HANDLE_COMPONENTS
        HANDLE_VERSION_RANGE)
endif()
