/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_ERROR_H_INCLUDED
#define SYSTEM_ERROR_H_INCLUDED

#include <memory>
#include <string>
#include <system_error>

#include <fmt/format.h>
#include <fmt/xchar.h>

#include <core/types.h>

namespace geo {

    /// General-purpose error type.
    struct Error {
        std::string description{};
        std::error_code code{};
        std::unique_ptr<const Error> cause{};

        /// Indicates whether the error is non-empty, i.e. any of its fields are non-empty.
        explicit operator bool() const { return !is_empty(); }

        /// Clears all of the error's fields.
        void clear();

        /// Indicates whether the error is empty, i.e., all of its fields are empty.
        bool is_empty() const;

        /// Recursively checks if the error or any of its nested causes has the specified error
        /// code.
        bool matches(const std::error_code& code) const;

        /// Recursively checks if the error of any of its nested causes satisfies the specified
        /// error condition.
        bool matches(const std::error_condition& cond) const;

        /// Writes a detailed error message to a `fmt::format_context`.
        fmt::format_context::iterator write_to(fmt::format_context& ctx) const;

#ifdef _WIN32
        /// Writes a detailed error message to a `fmt::wformat_context`.
        fmt::wformat_context::iterator write_to(fmt::wformat_context& ctx) const;
#endif
    };

    std::string to_string(const Error& error);

} // namespace geo

template<>
struct fmt::formatter<geo::Error, char> : formatter<std::string_view, char> {
    format_context::iterator format(const geo::Error& error, format_context& ctx) const
    {
        return error.write_to(ctx);
    }
};

#ifdef _WIN32
template<>
struct fmt::formatter<geo::Error, wchar_t> : formatter<std::wstring_view, wchar_t> {
    wformat_context::iterator format(const geo::Error& error, wformat_context& ctx) const
    {
        return error.write_to(ctx);
    }
};
#endif // defined(_WIN32_

#endif // SYSTEM_ERROR_H_INCLUDED
