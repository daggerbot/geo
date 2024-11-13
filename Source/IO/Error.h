/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_IO_ERROR_H_INCLUDED
#define GEO_IO_ERROR_H_INCLUDED

#include <System/Error.h>

namespace Geo {

    /// @addtogroup IO
    /// @{

    /// Common I/O error codes that may not have a perfectly corresponding `errno` value.
    enum class IoErrorCode {
        None,
        NotFound,
        StreamClosed,
        ArchiveClosed,
        EndOfStream,
        NotReadable,
        NotWritable,
        NotSeekable,
        SizeUndefined,
    };

    /// Error category corresponding to @ref IoErrorCode values.
    extern constinit const std::error_category& IoErrorCategory;

    /// Constructs an `std::error_code` from an @ref IoErrorCode.
    std::error_code make_error_code(IoErrorCode code);

    /// @}

} // namespace Geo

template<> struct std::is_error_code_enum<Geo::IoErrorCode> : true_type {};

#endif // GEO_IO_ERROR_H_INCLUDED
