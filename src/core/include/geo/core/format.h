/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_FORMAT_H_INCLUDED
#define GEO_CORE_FORMAT_H_INCLUDED

#include <optional>
#include <string>
#include <string_view>

#include "ascii.h"
#include "concepts.h"
#include "parse.h"
#include "static_string.h"
#include "string_utils.h"

namespace geo {

    /// @addtogroup geo_core
    /// @{

    /// Specializable type for formatting values as text.
    ///
    /// Proper specialization must declare at least the following members:
    /// ```
    /// Formatter();
    ///
    /// // Parse the `spec` part of the value sequence: `{[index:][spec]}`
    /// // Returns true on success.
    /// bool parse(std::basic_string_view<CharT> spec);
    ///
    /// // Emit a value by calling `writer` for each output character.
    /// template<std::invocable<CharT> Writer>
    /// void format(const T& value, Writer& writer) const;
    /// ```
    template<typename T, CoreChar CharT>
    struct Formatter;

    /// Concept for types that are formattable as text. This is determine by the presence of a
    /// proper specialization of @ref Formatter.
    template<typename T, typename CharT>
    concept Formattable =
        std::is_default_constructible_v<T>
        && requires(Formatter<T, CharT> formatter, std::basic_string_view<CharT> spec) {
            { formatter.parse(spec) } -> std::same_as<bool>;
        }
        && requires(const Formatter<T, CharT> formatter, const T& value, decltype([](CharT){})& writer) {
            { formatter.format(value, writer) };
        };

    /// Error code for string-formatting operations.
    enum class FormatStatus {
        /// The operation was successful.
        ok,

        /// The operation completed, but the `fmt` string contains errors.
        ok_with_invalid_fmt,
    };

    /// Text-formatting utilities.
    namespace format {

        namespace internal {

            /// Partially parsed value sequence.
            template<CoreChar CharT>
            struct ValueSequence {
                int index;
                std::basic_string_view<CharT> spec;

                constexpr ValueSequence<CharT> next() const
                {
                    return {index - 1, spec};
                }
            };

            /// Parses the `fmt` string, calling `charWriter` for each verbatim character, or
            /// `valueWriter` for each value sequence.
            template<CoreChar CharT,
                     std::invocable<CharT> CharWriter,
                     std::invocable<ValueSequence<CharT>> ValueWriter>
            constexpr FormatStatus parse(std::basic_string_view<CharT> fmt,
                                         CharWriter& charWriter,
                                         ValueWriter& valueWriter)
            requires std::same_as<std::invoke_result_t<ValueWriter, ValueSequence<CharT>>, bool>
            {
                const CharT* pos = fmt.data();
                const CharT* end = pos + fmt.length();
                int nextValueIndex = 0;
                bool hasErrors = false;

                while (pos != end) {
                    if (*pos == '{') {
                        const CharT* seqStart = pos++;

                        // Check for `{{` escape sequence.
                        if (pos != end && *pos == '{') {
                            charWriter(CharT('{'));
                            ++pos;
                            continue;
                        }

                        // Extract the value index.
                        bool seqOk = true;
                        int index = -1;

                        if (pos != end && ascii::is_digit(*pos)) {
                            auto result = parse::integer<int>(pos, size_t(end - pos));
                            pos += result.length;

                            if (result.value)
                                index = *result.value;
                            else
                                seqOk = false;
                        }

                        // Extract the value spec string.
                        std::basic_string_view<CharT> spec = {};

                        if (pos != end && *pos == ':') {
                            const CharT* specStart = ++pos;

                            while (pos != end && *pos != '{' && *pos != '}')
                                ++pos;

                            spec = {specStart, size_t(pos - specStart)};
                        }

                        // Emit the value if everything has worked so far.
                        if (seqOk && pos != end && *pos == '}') {
                            if (index < 0)
                                index = nextValueIndex++;

                            seqOk = valueWriter(ValueSequence<CharT>{index, spec});
                        } else {
                            seqOk = false;
                        }

                        // Move to the end of the sequence.
                        while (pos != end && *pos != '{' && *pos != '}') {
                            ++pos;
                        }
                        if (pos != end && *pos == '}') {
                            ++pos;
                        }

                        // Emit the sequence verbatim if anything went wrong.
                        if (!seqOk) {
                            while (seqStart != pos) {
                                charWriter(*seqStart++);
                            }
                            hasErrors = true;
                        }
                    } else if (*pos == '}') {
                        // Check for a `}}` escape sequence.
                        charWriter(CharT('}'));
                        ++pos;

                        if (pos != end && *pos == '}')
                            ++pos;
                        else
                            hasErrors = true;
                    } else {
                        // Emit a verbatim character.
                        charWriter(*pos++);
                    }
                }

                return hasErrors ? FormatStatus::ok_with_invalid_fmt : FormatStatus::ok;
            }

            /// Recursive type for handling variadic value arguments.
            template<typename...Values>
            struct ValueList;

            template<>
            struct ValueList<> {
                template<CoreChar CharT, std::invocable<CharT> Writer>
                constexpr bool format(ValueSequence<CharT>, Writer&) const
                {
                    return false;
                }

                template<CoreChar CharT>
                static constexpr bool validate(ValueSequence<CharT>)
                {
                    return false;
                }
            };

            template<typename Head, typename...Tail>
            struct ValueList<Head, Tail...> {
                const Head& head;
                ValueList<Tail...> tail;

                constexpr explicit ValueList(const Head& head, const Tail&...tail)
                    : head{head}, tail{tail...}
                {
                }

                template<CoreChar CharT, std::invocable<CharT> Writer>
                constexpr bool format(ValueSequence<CharT> seq, Writer& writer) const
                requires Formattable<Head, CharT>
                {
                    if (seq.index) {
                        return tail.format(seq.next(), writer);
                    } else {
                        Formatter<Head, CharT> formatter{};

                        if (!formatter.parse(seq.spec))
                            return false;

                        formatter.format(head, writer);
                        return true;
                    }
                }

                template<CoreChar CharT>
                static constexpr bool validate(ValueSequence<CharT> seq)
                requires Formattable<Head, CharT>
                {
                    if (seq.index)
                        return ValueList<Tail...>::validate(seq.next());
                    else
                        return Formatter<Head, CharT>{}.parse(seq.spec);
                }
            };

        } // namespace internal

        /// Checks whether the `fmt` string is valid for the specified value types.
        template<CoreChar CharT, Formattable<CharT>...Values>
        constexpr bool validate(std::basic_string_view<CharT> fmt)
        {
            auto charWriter = [](CharT){};
            auto valueWriter = [](internal::ValueSequence<CharT> seq) -> bool {
                return internal::ValueList<Values...>::validate(seq);
            };

            return internal::parse(fmt, charWriter, valueWriter) == FormatStatus::ok;
        }

        /// Formats a string by invoking `writer` for each output character.
        template<CoreChar CharT, std::invocable<CharT> Writer, Formattable<CharT>...Values>
        constexpr void write(Writer&& writer, std::basic_string_view<CharT> fmt, const Values&...values)
        {
            internal::ValueList<Values...> valueList{values...};
            auto valueWriter = [&](internal::ValueSequence<CharT> seq) -> bool {
                return valueList.format(seq, writer);
            };

            internal::parse(fmt, writer, valueWriter);
        }

        /// Formats a string by invoking `writer` for each output character.
        template<CoreChar CharT, std::invocable<CharT> Writer, Formattable<CharT>...Values>
        constexpr void write(Writer&& writer, const CharT* fmt, const Values&...values)
        {
            return format::write(std::move(writer), std::basic_string_view<CharT>{fmt}, values...);
        }

        /// Computes the length of a formatted string.
        template<CoreChar CharT, Formattable<CharT>...Values>
        constexpr size_t length(std::basic_string_view<CharT> fmt, const Values&...values)
        {
            size_t length = 0;
            auto writer = [&](CharT) { ++length; };

            format::write(writer, fmt, values...);
            return length;
        }

        /// Returns a formatted string.
        template<CoreChar CharT, Formattable<CharT>...Values>
        std::basic_string<CharT> string(std::basic_string_view<CharT> fmt, const Values&...values)
        {
            std::basic_string<CharT> str;
            auto writer = [&](CharT ch) { str.push_back(ch); };

            format::write(writer, fmt, values...);
            return str;
        }

        /// Returns a formatted string.
        template<CoreChar CharT, Formattable<CharT>...Values>
        std::basic_string<CharT> string(const CharT* fmt, const Values&...values)
        {
            return format::string(std::basic_string_view<CharT>{fmt}, values...);
        }

        /// Formats a string at compile time.
        template<StaticString Fmt, auto...Values>
        consteval auto static_string()
        requires (format::validate<typename decltype(Fmt)::char_type, decltype(Values)...>(Fmt.full_view()))
        {
            using char_type = typename decltype(Fmt)::char_type;

            constexpr std::basic_string_view<char_type> fmt = Fmt.full_view();
            constexpr size_t length = format::length(fmt, Values...);

            StaticString<char_type, length> str;
            size_t index = 0;
            auto writer = [&](char_type ch) { str[index++] = ch; };

            format::write(writer, fmt, Values...);
            return str;
        }

    } // namespace format

    //==============================================================================================
    // Character formatter

    template<CoreChar CharT>
    struct Formatter<CharT, CharT> {
        constexpr bool parse(std::basic_string_view<CharT> spec)
        {
            return spec.empty();
        }

        template<std::invocable<CharT> Writer>
        constexpr void format(CharT ch, Writer& writer) const
        {
            writer(ch);
        }
    };

    //==============================================================================================
    // String formatters

    template<CoreChar CharT>
    struct Formatter<std::basic_string_view<CharT>, CharT> {
        constexpr bool parse(std::basic_string_view<CharT> spec)
        {
            return spec.empty();
        }

        template<std::invocable<CharT> Writer>
        constexpr void format(std::basic_string_view<CharT> str, Writer& writer) const
        {
            for (CharT ch : str)
                writer(ch);
        }
    };

    template<CoreChar InChar, CoreChar OutChar>
    struct Formatter<InChar*, OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {};

    template<CoreChar InChar, CoreChar OutChar>
    struct Formatter<const InChar*, OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {};

    template<CoreChar InChar, size_t N, CoreChar OutChar>
    struct Formatter<InChar[N], OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {
        template<std::invocable<OutChar> Writer>
        constexpr void format(const InChar (&str)[N], Writer& writer) const
        {
            Formatter<std::basic_string_view<InChar>, OutChar>::format(string_utils::view_truncated(str), writer);
        }
    };

    template<CoreChar InChar, CoreChar OutChar>
    struct Formatter<std::basic_string<InChar>, OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {};

    //==============================================================================================
    // Boolean formatter

    template<CoreChar CharT>
    struct Formatter<bool, CharT> {
        constexpr bool parse(std::basic_string_view<CharT> spec)
        {
            return spec.empty();
        }

        template<std::invocable<CharT> Writer>
        constexpr void format(bool value, Writer& writer) const
        {
            for (const char* str = value ? "true" : "false"; *str; ++str)
                writer(CharT(*str));
        }
    };

    //==============================================================================================
    // Integer formatters

    struct IntFormatterBase {
        enum class Alignment { left, center, right };
        enum class DigitCase { lower, upper };
        enum class SignMode { normal, always, fill };

        static inline constexpr int min_radix = 2;
        static inline constexpr int max_radix = 36;

        Alignment alignment = Alignment::right;
        DigitCase digit_case = DigitCase::lower;
        unsigned digit_grouping = 3;
        int radix = 10;
        SignMode sign_mode = SignMode::normal;
        unsigned width = 0;
        bool zero_fill = false;
        bool zero_prefix = false;

        constexpr bool validate() const
        {
            switch (alignment) {
            case Alignment::left:
            case Alignment::center:
            case Alignment::right:
                break;
            default:
                return false;
            }

            switch (digit_case) {
            case DigitCase::lower:
            case DigitCase::upper:
                break;
            default:
                return false;
            }

            if (digit_grouping < 1)
                return false;
            else if (radix < min_radix || radix > max_radix)
                return false;

            switch (sign_mode) {
            case SignMode::normal:
            case SignMode::always:
            case SignMode::fill:
                break;
            default:
                return false;
            }

            return true;
        }
    };

    template<CoreChar CharT>
    struct IntFormatter : IntFormatterBase {
        CharT fill_char = ' ';
        std::basic_string_view<CharT> prefix = {};
        CharT separator = 0;

        constexpr bool parse(std::basic_string_view<CharT> spec)
        {
            const CharT* pos = spec.data();
            const CharT* end = pos + spec.length();

            // Restore default values.
            *this = {};

            // fill_char
            if (end - pos >= 2) {
                switch (pos[1]) {
                case '<':
                case '^':
                case '>':
                    fill_char = *pos++;
                    break;
                default:
                    break;
                }
            }

            // alignment
            if (pos != end) {
                switch (*pos) {
                case '<':
                    this->alignment = IntFormatterBase::Alignment::left;
                    ++pos;
                    break;
                case '^':
                    this->alignment = IntFormatterBase::Alignment::center;
                    ++pos;
                    break;
                case '>':
                    this->alignment = IntFormatterBase::Alignment::right;
                    ++pos;
                    break;
                default:
                    break;
                }
            }

            // sign_mode
            if (pos != end) {
                switch (*pos) {
                case '+':
                    this->sign_mode = IntFormatterBase::SignMode::always;
                    ++pos;
                    break;
                case '-':
                    this->sign_mode = IntFormatterBase::SignMode::normal;
                    ++pos;
                    break;
                case ' ':
                    this->sign_mode = IntFormatterBase::SignMode::fill;
                    ++pos;
                    break;
                default:
                    break;
                }
            }

            // use_prefix
            bool use_prefix = false;

            if (pos != end && *pos == '#') {
                use_prefix = true;
                ++pos;
            }

            // zero_fill
            if (pos != end && *pos == '0') {
                this->zero_fill = true;
                ++pos;
            }

            // width
            if (pos != end && ascii::is_digit(*pos)) {
                auto result = parse::integer<decltype(this->width)>(pos, size_t(end - pos));
                if (!result.value)
                    return false;
                this->width = *result.value;
                pos += result.length;
            }

            // separator
            if (pos != end && *pos == 'H') {
                separator = '\'';
                ++pos;
                if (end - pos >= 2 && *pos == ':') {
                    separator = pos[1];
                    pos += 2;
                }
            }

            // radix, digit_case, digit_grouping, prefix, zero_prefix
            if (pos != end) {
                switch (*pos) {
                case 'B':
                case 'b':
                    this->radix = 2;
                    this->digit_grouping = 4;
                    if (use_prefix) prefix = binary_prefix;
                    ++pos;
                    break;
                case 'O':
                case 'o':
                    this->radix = 8;
                    this->digit_grouping = 4;
                    if (use_prefix) this->zero_prefix = true;
                    ++pos;
                    break;
                case 'D':
                case 'I':
                case 'd':
                case 'i':
                    break;
                case 'X':
                case 'x':
                    this->radix = 16;
                    this->digit_case = *pos == 'X' ? IntFormatterBase::DigitCase::upper : IntFormatterBase::DigitCase::lower;
                    this->digit_grouping = 4;
                    if (use_prefix) prefix = hex_prefix;
                    ++pos;
                    break;
                default:
                    break;
                }
            }

            return pos == end;
        }

    protected:
        static constexpr CharT binary_prefix[] = {'0', 'b', 0};
        static constexpr CharT hex_prefix[] = {'0', 'x', 0};

        template<std::invocable<CharT> Writer>
        constexpr void write_digit(int digit, Writer& writer) const
        {
            if (digit < 10)
                writer(CharT('0' + digit));
            else if (this->digit_case == IntFormatterBase::DigitCase::upper)
                writer(CharT('A' - 10 + digit));
            else
                writer(CharT('a' - 10 + digit));
        }
    };

    template<StrictInt T, CoreChar CharT>
    struct Formatter<T, CharT> : IntFormatter<CharT> {
        template<std::invocable<CharT> Writer>
        constexpr void format(T value, Writer& writer) const
        {
            if (!this->validate()) {
                writer(CharT('?'));
                return;
            }

            auto m = measure(value);

            while (m.left_fill--)
                writer(this->fill_char);
            if (m.sign_char)
                writer(m.sign_char);
            for (CharT ch : this->prefix)
                writer(ch);

            write_digits_and_separators(value, writer, 0, m.n_digits_and_separators - 1);

            while (m.right_fill--)
                writer(this->fill_char);
        }

    private:
        struct Measurements {
            unsigned left_fill = 0;
            CharT sign_char = 0;
            unsigned n_digits_and_separators = 0;
            unsigned right_fill = 0;
        };

        constexpr Measurements measure(T value) const
        {
            Measurements m = {};

            // Determine sign character
            if (value < 0) {
                m.sign_char = '-';
            } else {
                switch (this->sign_mode) {
                case IntFormatterBase::SignMode::always:
                    m.sign_char = '+';
                    break;
                case IntFormatterBase::SignMode::fill:
                    m.sign_char = this->fill_char;
                    break;
                default:
                    break;
                }
            }

            unsigned sign_width = m.sign_char ? 1 : 0;

            // Count digits
            T accum = value;

            m.n_digits_and_separators = 1;

            if constexpr(std::is_signed_v<T>) {
                if (accum <= T(-this->radix)) {
                    ++m.n_digits_and_separators;
                    accum /= T(-this->radix);
                }
            }

            while (accum >= T(this->radix)) {
                ++m.n_digits_and_separators;
                accum /= T(this->radix);
            }

            if (value && this->zero_prefix)
                ++m.n_digits_and_separators;

            // Count separators
            if (this->separator) {
                m.n_digits_and_separators += (m.n_digits_and_separators - 1) / this->digit_grouping;
            }

            // Fill specified width
            if (this->zero_fill && this->width > sign_width + this->prefix.length() + m.n_digits_and_separators) {
                m.n_digits_and_separators = unsigned(this->width - sign_width - this->prefix.length());

                // Never start with a separator
                if (this->separator && m.n_digits_and_separators % (this->digit_grouping + 1) == 0)
                    --m.n_digits_and_separators;
            }

            // Determine left and right fill
            if (this->width > sign_width + this->prefix.length() + m.n_digits_and_separators) {
                unsigned fill = unsigned(this->width - sign_width - this->prefix.length() - m.n_digits_and_separators);

                switch (this->alignment) {
                case IntFormatterBase::Alignment::left:
                    m.right_fill = fill;
                    break;
                case IntFormatterBase::Alignment::center:
                    m.right_fill = (fill + sign_width) / 2;
                    m.left_fill = fill - m.right_fill;
                    break;
                case IntFormatterBase::Alignment::right:
                    m.left_fill = fill;
                    break;
                default:
                    break;
                }
            }

            return m;
        }

        template<std::invocable<CharT> Writer>
        constexpr void write_digits_and_separators(T value, Writer& writer, unsigned depth, unsigned remaining) const
        {
            if (this->separator && depth % (this->digit_grouping + 1) == this->digit_grouping) {
                write_digits_and_separators(value, writer, depth + 1, remaining - 1);
                writer(this->separator);
            } else if (value < 0) {
                if (remaining)
                    write_digits_and_separators(T(value / T(-this->radix)), writer, depth + 1, remaining - 1);
                this->write_digit(-int(value % T(-this->radix)), writer);
            } else {
                if (remaining)
                    write_digits_and_separators(T(value / T(this->radix)), writer, depth + 1, remaining - 1);
                this->write_digit(int(value % T(this->radix)), writer);
            }
        }
    };

    //==============================================================================================
    // Pointer formatters

    template<CoreChar CharT>
    struct Formatter<void*, CharT> : Formatter<uptr, CharT> {
        constexpr bool parse(std::basic_string_view<CharT> spec)
        {
            if (spec.empty())
                spec = fallback_spec;

            return Formatter<uptr, CharT>::parse(spec);
        }

    private:
        static const CharT fallback_spec = {'#', 'x', 0};
    };

    template<CoreChar CharT>
    struct Formatter<const void*, CharT> : Formatter<void*, CharT> {};

    /// @}

} // namespace geo

#endif // GEO_CORE_FORMAT_H_INCLUDED
