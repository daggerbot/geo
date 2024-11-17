/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef IO_ERROR_H_INCLUDED
#define IO_ERROR_H_INCLUDED

#include <system/error.h>

namespace geo {

    /// High-level I/O error codes.
    enum class IoErrorCode {
        none,
        not_found,
        stream_closed,
        archive_closed,
        not_readable,
        not_writable,
        not_seekable,
        end_of_stream,
        stream_size_undefined,
        stream_too_long,
    };

    /// Error category corresponding to @ref IoErrorCode values.
    extern constinit const std::error_category& io_error_category;

    std::error_code make_error_code(IoErrorCode code);

} // namespace geo

template<> struct std::is_error_code_enum<geo::IoErrorCode> : true_type {};

#endif // IO_ERROR_H_INCLUDED
