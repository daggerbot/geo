/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_SYSTEM_SYSTEM_H_INCLUDED
#define GEO_SYSTEM_SYSTEM_H_INCLUDED

#include <vector>

#include <geo/core/string_types.h>

/// @addtogroup geo_system
/// @{

/// Platform-dependent path separator string.
#ifdef _WIN32
# define PATH_SEPARATOR "\\"
#else
# define PATH_SEPARATOR "/"
#endif

/// @}

namespace geo {

    /// @addtogroup geo_system
    /// @{

    using ErrorDialogHandler = void (*)(const oschar_t*);

    /// OS functions.
    namespace system {

        /// Shows an error message in a way that makes sense on the current platform, then exits
        /// with an error code.
        ///
        /// In general, you should use the @ref FATAL macro for fatal errors, but this is available
        /// as a backup for situations where the logging system may be unavailable or
        /// malfunctioning.
        [[noreturn]] void exit_with_error_message(const oschar_t* message);

        /// Returns a list of directories to search for game data.
        std::vector<OsString> get_data_dirs();

        /// Initializes the system GUI. This may change the error dialog handler.
        /// @see @ref set_error_dialog_handler
        void init_gui();

        /// Sets the global error dialog handler.
        /// @see @ref show_error_dialog
        void set_error_dialog_handler(ErrorDialogHandler handler);

        /// Returns an error message corresponding to an `errno` value.
        std::string strerror_a(int errnum);

        /// Shows an error message dialog if available.
        /// @see @ref init_gui
        /// @see @ref set_error_dialog_handler
        void show_error_dialog(const oschar_t* message);

    } // namespace system

    /// @}

} // namespace geo

#endif // GEO_SYSTEM_SYSTEM_H_INCLUDED
