/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_PARSE_H_INCLUDED
#define GEO_CORE_PARSE_H_INCLUDED

#include <limits>
#include <optional>
#include <string_view>

#include "ascii.h"
#include "concepts.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Represents the outcome of a parsing operation.
    enum class ParseStatus {
        /// The operation was successful.
        ok,

        /// The input string is empty.
        empty,

        /// The input string was not understood.
        invalid_input,

        /// The parsing options are invalid.
        invalid_options,

        /// The value could not be represented by the output type due to overflow.
        overflow,

        /// The value could not be represented by the output type due to underflow.
        underflow,
    };

    /// Return type for parsing operations.
    template<typename T>
    struct ParseResult {
        /// Describes whether the parsing operation succeeded or failed.
        ParseStatus status = ParseStatus::ok;

        /// The parsed value of type `T`, if available. If the parsing operation failed, this will
        /// be `std::nullopt`.
        std::optional<T> value;

        /// The number of characters from the input string that were successfully parsed.
        size_t length = 0;

        /// Indicates whether there is unparsed input data remaining after the parsed portion. If
        /// true, the parser successfully parsed part of the input string but additional data
        /// follows that was not consumed during the operation.
        bool has_trailing_input = false;

        constexpr bool operator==(const ParseResult<T>& rhs) const
        {
            return status == rhs.status
                   && value == rhs.value
                   && length == rhs.length
                   && has_trailing_input == rhs.has_trailing_input;
        }
    };

    /// Additional options for parsing integer values.
    struct ParseIntOptions {
        static inline constexpr int min_radix = 2;
        static inline constexpr int max_radix = 36;

        /// If true, a sign prefix can determine whether the value is positive (`+`) or negative
        /// (`-`).
        bool allow_sign = true;

        /// Numeric base used when parsing digits. Can be a value from @ref min_radix to
        /// @ref max_radix, or can be zero to allow a prefix to specify the base:
        /// - `0b` for binary (base 2),
        /// - `0` for octal (base 8),
        /// - `0x` for hexadecimal (base 16), or
        /// - decimal (base 10) by default.
        int radix = 10;

        /// If non-zero, this character is interpreted as a digit separator. Must not be an
        /// alphanumeric ASCII character.
        u32 separator = 0;

        constexpr bool validate() const
        {
            if (radix && (radix < min_radix || radix > max_radix))
                return false;
            else if (ascii::is_alphanumeric(separator))
                return false;

            return true;
        }
    };

    /// Functions for parsing values from strings.
    namespace parse {

        namespace internal {

            template<CoreInt CharT>
            constexpr int get_digit_value(CharT ch)
            {
                if (ascii::is_digit(ch))
                    return int(ch - '0');
                else if (ascii::is_uppercase(ch))
                    return int(ch - 'A' + 10);
                else if (ascii::is_lowercase(ch))
                    return int(ch - 'a' + 10);
                else
                    return -1;
            }

        } // namespace internal

        /// Parses an integer from the front of the input string.
        template<CoreInt T, CoreInt CharT>
        constexpr ParseResult<T> integer(std::basic_string_view<CharT> str, const ParseIntOptions& options = {})
        {
            const CharT* pos = str.data();
            const CharT* end = pos + str.length();

            if (!options.validate())
                return {ParseStatus::invalid_options, {}, 0, false};
            else if (str.empty())
                return {ParseStatus::empty, {}, 0, false};

            // Handle the sign prefix if present.
            bool negative = false;

            if (pos != end && options.allow_sign) {
                switch (*pos) {
                case '+':
                    ++pos;
                    break;
                case '-':
                    negative = true;
                    ++pos;
                    break;
                default:
                    break;
                }
            }

            // Determine the radix from options or from a prefix.
            int radix;
            bool has_digits = false;

            if (options.radix) {
                radix = options.radix;
            } else if (pos != end && *pos == '0') {
                radix = 8;
                has_digits = true;
                ++pos;

                if (pos != end) {
                    switch (*pos) {
                    case 'B':
                    case 'b':
                        radix = 2;
                        has_digits = false;
                        ++pos;
                        break;
                    case 'X':
                    case 'x':
                        radix = 16;
                        has_digits = false;
                        ++pos;
                        break;
                    default:
                        break;
                    }
                }
            } else {
                radix = 10;
            }

            // Parse digits and separators.
            ParseStatus status = ParseStatus::ok;
            T value = 0;

            while (pos != end) {
                // Skip a separator
                const CharT* digitPos = pos;
                if (options.separator && std::make_unsigned_t<CharT>(*pos) == options.separator && has_digits)
                    if (++digitPos == end)
                        break;

                // Get the value of the next digit
                int digit = internal::get_digit_value(*digitPos);
                if (digit < 0 || digit >= radix)
                    break;
                has_digits = true;
                pos = digitPos + 1;

                // Incorporate the digit into our final value
                if (status == ParseStatus::ok) {
                    if (negative) {
                        if constexpr(std::is_signed_v<T>) {
                            if (value < (std::numeric_limits<T>::min() + T(digit)) / T(radix)) {
                                status = ParseStatus::underflow;
                                continue;
                            }
                            value = T(value * T(radix) - T(digit));
                        } else if (digit) {
                            status = ParseStatus::underflow;
                            continue;
                        }
                    } else {
                        if (value > (std::numeric_limits<T>::max() - T(digit)) / T(radix)) {
                            status = ParseStatus::overflow;
                            continue;
                        }
                        value = T(value * T(radix) + T(digit));
                    }
                }
            }

            // Final checks.
            if (!has_digits)
                return {ParseStatus::invalid_input, {}, 0, false};
            else if (status != ParseStatus::ok)
                return {status, {}, size_t(pos - str.data()), pos != end};
            else
                return {ParseStatus::ok, value, size_t(pos - str.data()), pos != end};
        }

        /// Parses an integer from the front of the input string.
        template<CoreInt T, CoreInt CharT>
        constexpr ParseResult<T> integer(const CharT* str, const ParseIntOptions& options = {})
        {
            return parse::integer<T>(std::basic_string_view<CharT>{str}, options);
        }

        /// Parses an integer from the front of the input string.
        template<CoreInt T, CoreInt CharT>
        constexpr ParseResult<T> integer(const CharT* str, size_t len, const ParseIntOptions& options = {})
        {
            return parse::integer<T>(std::basic_string_view<CharT>{str, len}, options);
        }

    } // namespace parse

    /// @}

} // namespace geo

#endif // GEO_CORE_PARSE_H_INCLUDED
