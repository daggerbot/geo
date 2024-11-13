/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <Core/Format.h>

#include "Error.h"

using namespace Geo;

namespace {

    class IoErrorCategoryType : public std::error_category {
    public:
        std::string message(int value) const override
        {
            switch (IoErrorCode(value)) {
                case IoErrorCode::None: return "No error";
                case IoErrorCode::NotFound: return "File not found";
                case IoErrorCode::StreamClosed: return "Stream is closed";
                case IoErrorCode::ArchiveClosed: return "Archive is closed";
                case IoErrorCode::EndOfStream: return "Unexpected end of stream";
                case IoErrorCode::NotReadable: return "Stream is not readable";
                case IoErrorCode::NotWritable: return "Stream is not writable";
                case IoErrorCode::NotSeekable: return "Stream is not seekable";
                case IoErrorCode::SizeUndefined: return "Stream has no defined size";
                default: return Format::String<char>("IoErrorCode {}", value);
            }
        }

        const char* name() const noexcept override { return "IoErrorCode"; }
    } constinit const s_ioErrorCategory;

} // namespace

constinit const std::error_category& Geo::IoErrorCategory = s_ioErrorCategory;

std::error_code Geo::make_error_code(IoErrorCode code)
{
    return {int(code), IoErrorCategory};
}
