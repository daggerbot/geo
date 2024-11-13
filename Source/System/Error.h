/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_SYSTEM_ERROR_H_INCLUDED
#define GEO_SYSTEM_ERROR_H_INCLUDED

#include <memory>
#include <string>
#include <system_error>

#include <Core/Concepts.h>

namespace Geo {

    template<CoreChar> class FormatWriter;
    template<typename, CoreChar> struct Formatter;

    /// @addtogroup System
    /// @{

    /// General-purpose error type.
    struct Error {
        std::string description{};
        std::error_code code{};
        std::unique_ptr<const Error> innerError{};

        /// Determines whether the error is non-empty, i.e., any of its fields are non-empty.
        explicit operator bool() const { return !IsEmpty(); }

        /// Clears all of the error's fields.
        void Clear();

        /// Returns a string describing the error, including all of its fields.
        std::string GetFullMessage() const;

        /// Indicates whether the error is empty, i.e., all of its fields are empty.
        bool IsEmpty() const;

        /// Writes the detailed error message to `writer`.
        void WriteTo(FormatWriter<char>& writer) const;
    };

    /// `Error` formatter.
    template<>
    struct Formatter<Error, char> {
        constexpr bool Parse(std::string_view spec)
        {
            return spec.empty();
        }

        void Format(const Error& error, FormatWriter<char>& writer) const
        {
            error.WriteTo(writer);
        }
    };

    /// Converting `Error` formatter.
    template<CoreChar CharT>
    struct Formatter<Error, CharT> : Formatter<std::string, CharT> {
        void Format(const Error& error, FormatWriter<CharT>& writer) const
        {
            Formatter<std::string, CharT>::Format(error.GetFullMessage(), writer);
        }
    };

    /// @}

} // namespace Geo

#endif // GEO_SYSTEM_ERROR_H_INCLUDED
