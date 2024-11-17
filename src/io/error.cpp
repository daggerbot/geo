/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <fmt/format.h>

#include "error.h"

using namespace geo;

namespace {

    class IoErrorCategory : public std::error_category {
        bool equivalent(int value, const std::error_condition& cond) const noexcept override
        {
            switch (IoErrorCode(value)) {
                case IoErrorCode::not_found:
                    return cond == std::errc::no_such_file_or_directory;
                case IoErrorCode::stream_closed:
                case IoErrorCode::archive_closed:
                    return cond == std::errc::bad_file_descriptor;
                case IoErrorCode::not_readable:
                case IoErrorCode::not_writable:
                case IoErrorCode::not_seekable:
                    return cond == std::errc::not_supported;
                case IoErrorCode::stream_too_long:
                    return cond == std::errc::file_too_large;
                default:
                    return false;
            }
        }

        std::string message(int value) const override
        {
            switch (IoErrorCode(value)) {
                case IoErrorCode::none: return "No error";
                case IoErrorCode::not_found: return "File not found";
                case IoErrorCode::stream_closed: return "Stream is closed";
                case IoErrorCode::archive_closed: return "Archive is closed";
                case IoErrorCode::not_readable: return "Stream is not readable";
                case IoErrorCode::not_writable: return "Stream is not writable";
                case IoErrorCode::not_seekable: return "Stream is not seekable";
                case IoErrorCode::end_of_stream: return "Unexpected end of stream";
                case IoErrorCode::stream_size_undefined: return "Stream size is not defined";
                case IoErrorCode::stream_too_long: return "Stream exceeds maximum size";
                default: return fmt::format("I/O error code {}", value);
            }
        }

        const char* name() const noexcept override { return "IoErrorCode"; }
    } constinit const io_error_category_instance;

} // namespace

constinit const std::error_category& geo::io_error_category = io_error_category_instance;

std::error_code geo::make_error_code(IoErrorCode code)
{
    return {int(code), io_error_category};
}
