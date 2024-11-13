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
#include <string_view>

#include "Ascii.h"
#include "Concepts.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// Represents the result of a parsing operation, indicating success or the reason for failure.
    enum class ParseStatus {
        Ok, ///< The operation was successful.
        Empty, ///< The input string is empty.
        InvalidInput, ///< The input string is invalid.
        InvalidOptions, ///< The provided parsing options are invalid.
        Overflow, ///< The parsed value cannot be represented due to overflow.
        Underflow, ///< The parsed value cannot be represented due to underflow.
    };

    /// Return type for value-parsing operations.
    template<typename T>
    struct ParseResult {
        ParseStatus status = ParseStatus::Ok; ///< Indicates success or the reason for failure.
        T value = 0; ///< The parsed value, if successful.
        size_t length = 0; ///< Length of the parsed portion of the input string.
        bool hasTrailingInput = false; ///< True if there is unparsed input after the parsed portion.

        constexpr bool operator==(const ParseResult<T>& rhs) const
        {
            return status == rhs.status
                   && value == rhs.value
                   && length == rhs.length
                   && hasTrailingInput == rhs.hasTrailingInput;
        }
    };

    /// Options for parsing integer values from strings.
    struct ParseIntOptions {
        static inline constexpr int MinRadix = 2; ///< Minimum allowed @ref radix, other than 0.
        static inline constexpr int MaxRadix = 36; ///< Maximum allowed @ref radix.

        /// If true, allows a sign prefix (`+` or `-`) to indicate positive or negative values.
        bool allowSign = true;

        /// If non-zero, specifies a character (code unit) used as a digit separator, e.g., a
        /// thousands separator. This must not be an alphanumeric ASCII character. Note that in
        /// multibyte encodings like UTF-8, `digitSeparator` should be an ASCII character, as it is
        /// interpreted as a single code unit, not a Unicode scalar or code point.
        u32 digitSeparator = 0;

        /// Numeric base used for parsing digits. Must be within @ref MinRadix to @ref MaxRadix,
        /// or zero to infer the base from a prefix:
        /// - `0b` for binary (base 2)
        /// - `0` for octal (base 8)
        /// - `0x` for hexadecimal (base 16)
        /// - No prefix defaults to decimal (base 10)
        int radix = 10;

        /// Checks if the current parsing options are valid.
        constexpr bool Validate() const
        {
            if (radix && (radix < MinRadix || radix > MaxRadix))
                return false;
            else if (Ascii::IsAlphanumeric(digitSeparator))
                return false;

            return true;
        }
    };

    /// Functions for parsing values from strings.
    namespace Parse {

        /// Parses an integer value from the front of the input string.
        template<CoreInt T, CoreInt CharT>
        constexpr ParseResult<T> Int(std::basic_string_view<CharT> str, const ParseIntOptions& options = {})
        {
            const CharT* pos = str.data();
            const CharT* end = pos + str.length();

            if (!options.Validate())
                return {.status = ParseStatus::InvalidOptions};
            else if (str.empty())
                return {.status = ParseStatus::Empty};

            // Handle a sign char.
            bool isNegative = false;

            if (options.allowSign) {
                switch (*pos) {
                    case '+':
                        ++pos;
                        break;
                    case '-':
                        isNegative = true;
                        ++pos;
                        break;
                    default:
                        break;
                }
            }

            // Determine the radix from `options` or from a prefix.
            int radix;
            bool hasAnyDigits = false;

            if (options.radix) {
                radix = options.radix;
            } else if (pos != end && *pos == '0') {
                radix = 8;
                hasAnyDigits = true;
                ++pos;

                if (pos != end) {
                    switch (*pos) {
                        case 'B':
                        case 'b':
                            radix = 2;
                            hasAnyDigits = false;
                            ++pos;
                            break;
                        case 'X':
                        case 'x':
                            radix = 16;
                            hasAnyDigits = false;
                            ++pos;
                            break;
                        default:
                            break;
                    }
                }
            } else {
                radix = 10;
            }

            // Handle digits and separators.
            ParseStatus status = ParseStatus::Ok;
            T value = 0;

            while (pos != end) {
                // Skip a valid separator.
                const CharT* digitPos = pos;

                if (options.digitSeparator
                    && std::make_unsigned_t<CharT>(*pos) == options.digitSeparator
                    && hasAnyDigits)
                {
                    if (++digitPos == end)
                        break;
                }

                // Get the digit value.
                int digit = Ascii::GetAlphanumericDigitValue(*digitPos);

                if (digit < 0 || digit >= radix)
                    break;

                hasAnyDigits = true;
                pos = digitPos + 1;

                // Add the digit into our final value.
                if (status == ParseStatus::Ok) {
                    if (isNegative) {
                        if constexpr(std::is_signed_v<T>) {
                            if (value < (std::numeric_limits<T>::min() + T(digit)) / T(radix)) {
                                status = ParseStatus::Underflow;
                                continue;
                            }
                            value = T(value * T(radix) - T(digit));
                        } else if (digit) {
                            status = ParseStatus::Underflow;
                            continue;
                        }
                    } else {
                        if (value > (std::numeric_limits<T>::max() - T(digit)) / T(radix)) {
                            status = ParseStatus::Overflow;
                            continue;
                        }
                        value = T(value * T(radix) + T(digit));
                    }
                }
            }

            // Final checks
            if (!hasAnyDigits)
                return {.status = ParseStatus::InvalidInput};

            return {
                .status = status,
                .value = status == ParseStatus::Ok ? value : T(0),
                .length = size_t(pos - str.data()),
                .hasTrailingInput = pos != end,
            };
        }

        /// Parses an integer value from the front of the input string.
        template<CoreInt T, CoreInt CharT>
        constexpr ParseResult<T> Int(const CharT* str, const ParseIntOptions& options = {})
        {
            return Parse::Int<T>(std::basic_string_view<CharT>{str}, options);
        }

        /// Parses an integer value from the front of the input string.
        template<CoreInt T, CoreInt CharT>
        constexpr ParseResult<T> Int(const CharT* str, const CharT* end, const ParseIntOptions& options = {})
        {
            return Parse::Int<T>(std::basic_string_view<CharT>{str, size_t(end - str)}, options);
        }

    } // namespace Parse

    /// @}

} // namespace Geo

#endif // GEO_CORE_PARSE_H_INCLUDED
