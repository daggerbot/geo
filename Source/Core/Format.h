/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_FORMAT_H_INCLUDED
#define GEO_CORE_FORMAT_H_INCLUDED

#include <string>

#include "Ascii.h"
#include "Concepts.h"
#include "Parse.h"
#include "StaticString.h"
#include "Str.h"

namespace Geo {

    /// @addtogroup Core
    /// @{

    /// Writes character data to a destination. Used when formatting strings.
    template<CoreChar CharT>
    class FormatWriter {
    public:
        /// Emits any buffered output characters.
        constexpr void Flush()
        {
            if (!m_used)
                return;

            EmitChars({m_buffer, m_used});
            m_used = 0;
        }

        /// Writes a single character.
        constexpr void Write(CharT ch)
        {
            if (m_used == BufferSize)
                Flush();

            m_buffer[m_used++] = ch;
        }

        /// Writes a character repeatedly.
        constexpr void Write(size_t count, CharT ch)
        {
            while (count--)
                Write(ch);
        }

        /// Writes a string.
        constexpr void Write(std::basic_string_view<CharT> str)
        {
            if (str.length() > BufferSize - m_used)
                Flush();

            if (str.length() >= BufferSize) {
                EmitChars(str);
            } else {
                for (CharT ch : str)
                    m_buffer[m_used++] = ch;
            }
        }

    protected:
        /// Emits characters to the destination.
        constexpr virtual void EmitChars(std::basic_string_view<CharT> chars) = 0;

    private:
        static constexpr size_t BufferSize = 120;

        CharT m_buffer[BufferSize];
        size_t m_used = 0;
    };

    /// Specializable type for formatting values as text.
    ///
    /// Specializations should define at least the following members:
    /// ```
    /// Formatter();
    /// bool Parse(std::basic_string_view<CharT> spec);
    /// void Format(const T& value, FormatWriter<CharT>& writer) const;
    /// ```
    /// If the default constructor and `Parse` are `constexpr`, then format strings using `T` values
    /// can be validated at compile time. If the default constructor, `Parse`, and `Format` are all
    /// `constexpr`, then format strings using `T` values can be formatted at compile time.
    template<typename T, CoreChar CharT>
    struct Formatter;

    /// Concept for types that are formattable as strings of `CharT`. This is determined by the
    /// presence of a proper specialization of @ref Formatter.
    template<typename T, typename CharT>
    concept Formattable =
        std::is_default_constructible_v<T>
        && requires(Formatter<T, CharT>& formatter, std::basic_string_view<CharT> spec) {
            { formatter.Parse(spec) } -> std::same_as<bool>;
        } && requires(const Formatter<T, CharT>& formatter, const T& value, FormatWriter<CharT>& writer) {
            { formatter.Format(value, writer) } -> std::same_as<void>;
        };

    /// Indicates whether string formatting was successful.
    enum class FormatStatus {
        Ok, ///< Formatting was successful.
        CompletedWithErrors, ///< Formatting completed, but invalid sequences were encountered and emitted verbatim.
    };

    /// String formatting utilities.
    namespace Format {

        namespace Internal {

            /// Format writer that discards all output.
            template<CoreChar CharT>
            class NullFormatWriter : public FormatWriter<CharT> {
            protected:
                constexpr void EmitChars(std::basic_string_view<CharT>) override {}
            };

            /// Format writer that invokes a callback when output data becomes available.
            template<CoreChar CharT, std::invocable<std::basic_string_view<CharT>> Callback>
            class CallbackFormatWriter : public FormatWriter<CharT> {
            public:
                constexpr explicit CallbackFormatWriter(Callback& callback) : m_callback{callback} {}

            protected:
                constexpr void EmitChars(std::basic_string_view<CharT> chars) override { m_callback(chars); }

            private:
                Callback& m_callback;
            };

            /// Interface for emitting partially parsed format value sequences.
            template<CoreChar CharT>
            class ValueEmitter {
            public:
                constexpr virtual bool EmitValue(int index, std::basic_string_view<CharT> spec) = 0;
            };

            /// Value emitter that only validates value sequences.
            template<CoreChar CharT, Formattable<CharT>...Values>
            class ValidateValueEmitter;

            template<CoreChar CharT>
            class ValidateValueEmitter<CharT> : public ValueEmitter<CharT> {
            public:
                constexpr bool EmitValue(int, std::basic_string_view<CharT>) override { return false; }
            };

            template<CoreChar CharT, Formattable<CharT> Head, Formattable<CharT>...Tail>
            class ValidateValueEmitter<CharT, Head, Tail...> : public ValueEmitter<CharT> {
            public:
                constexpr bool EmitValue(int index, std::basic_string_view<CharT> spec) override
                {
                    if (index)
                        return m_tail.EmitValue(index - 1, spec);
                    else
                        return Formatter<Head, CharT>{}.Parse(spec);
                }

            private:
                [[no_unique_address]] ValidateValueEmitter<CharT, Tail...> m_tail;
            };

            /// Value emitter that formats values using a writer.
            template<CoreChar CharT, Formattable<CharT>...Values>
            class FormatValueEmitter;

            template<CoreChar CharT>
            class FormatValueEmitter<CharT> : public ValueEmitter<CharT> {
            public:
                constexpr explicit FormatValueEmitter(FormatWriter<CharT>& writer) : m_writer{writer} {}

                constexpr bool EmitValue(int, std::basic_string_view<CharT>) override { return false; }
                constexpr FormatWriter<CharT>& GetWriter() { return m_writer; }

            private:
                FormatWriter<CharT>& m_writer;
            };

            template<CoreChar CharT, Formattable<CharT> Head, Formattable<CharT>...Tail>
            class FormatValueEmitter<CharT, Head, Tail...> : public ValueEmitter<CharT> {
            public:
                constexpr explicit FormatValueEmitter(const Head& head, const Tail&...tail, FormatWriter<CharT>& writer)
                    : m_head{head}, m_tail{tail..., writer}
                {
                }

                constexpr bool EmitValue(int index, std::basic_string_view<CharT> spec) override
                {
                    if (index) {
                        return m_tail.EmitValue(index - 1, spec);
                    } else {
                        Formatter<Head, CharT> formatter{};

                        if (!formatter.Parse(spec))
                            return false;

                        formatter.Format(m_head, GetWriter());
                        return true;
                    }
                }

                constexpr FormatWriter<CharT>& GetWriter() { return m_tail.GetWriter(); }

            private:
                const Head& m_head;
                FormatValueEmitter<CharT, Tail...> m_tail;
            };

            /// Parses and executes the format string.
            template<CoreChar CharT>
            constexpr FormatStatus DoFormat(std::basic_string_view<CharT> fmt,
                                            FormatWriter<CharT>& writer,
                                            ValueEmitter<CharT>& valueEmitter)
            {
                const CharT* pos = fmt.data();
                const CharT* end = pos + fmt.length();
                int nextValueIndex = 0;
                FormatStatus status = FormatStatus::Ok;

                while (pos != end) {
                    if (*pos == '{') {
                        const CharT* seqStart = pos++;

                        // Check for a `{{` escape sequence.
                        if (pos != end && *pos == '{') {
                            writer.Write(CharT('{'));
                            ++pos;
                            continue;
                        }

                        // Extract the value index if present.
                        bool seqOk = true;
                        int index = -1;

                        if (pos != end && Ascii::IsDigit(*pos)) {
                            auto result = Parse::Int<int>(pos, end);

                            if (result.status == ParseStatus::Ok)
                                index = result.value;
                            else
                                seqOk = false;

                            pos += result.length;
                        }

                        // Extract the spec string if present.
                        std::basic_string_view<CharT> spec = {};

                        if (pos != end && *pos == ':') {
                            const CharT* specStart = ++pos;

                            while (pos != end && *pos != '{' && *pos != '}')
                                ++pos;

                            spec = {specStart, size_t(pos - specStart)};
                        }

                        // Emit the value sequence if everything went right so far.
                        if (seqOk && pos != end && *pos == '}') {
                            if (index < 0)
                                index = nextValueIndex++;

                            if (valueEmitter.EmitValue(index, spec)) {
                                ++pos;
                                continue;
                            }
                        }

                        // If we made it this far, something went wrong.
                        // Emit the sequence verbatim and report the error.
                        while (pos != end && *pos != '{' && *pos != '}')
                            ++pos;

                        if (pos != end && *pos == '}')
                            ++pos;

                        while (seqStart != pos)
                            writer.Write(*seqStart++);

                        status = FormatStatus::CompletedWithErrors;
                    } else if (*pos == '}') {
                        // Check for a `}}` escape sequence.
                        writer.Write(CharT('}'));
                        ++pos;

                        if (pos != end && *pos == '}')
                            ++pos;
                        else
                            status = FormatStatus::CompletedWithErrors;
                    } else {
                        // Not part of a sequence; emit the character verbatim.
                        writer.Write(*pos++);
                    }
                }

                writer.Flush();
                return status;
            }

        } // namespace Internal

        /// Validates the format string given the specified value types.
        template<CoreChar CharT, Formattable<CharT>...Values>
        constexpr bool Validate(std::basic_string_view<CharT> fmt)
        {
            Internal::NullFormatWriter<CharT> writer;
            Internal::ValidateValueEmitter<CharT, Values...> valueEmitter;
            return Internal::DoFormat(fmt, writer, valueEmitter) == FormatStatus::Ok;
        }

        /// Formats a string by writing output to a @ref FormatWriter.
        template<CoreChar CharT, Formattable<CharT>...Values>
        constexpr FormatStatus WriteTo(FormatWriter<CharT>& writer,
                                       std::basic_string_view<CharT> fmt,
                                       const Values&...values)
        {
            Internal::FormatValueEmitter<CharT, Values...> valueEmitter{values..., writer};
            return Internal::DoFormat(fmt, writer, valueEmitter);
        }

        /// Formats a string by writing output to a @ref FormatWriter.
        template<CoreChar CharT, Formattable<CharT>...Values>
        constexpr FormatStatus WriteTo(FormatWriter<CharT>& writer, const CharT* fmt, const Values&...values)
        {
            return Format::WriteTo(writer, std::basic_string_view<CharT>{fmt}, values...);
        }

        /// Formats a string by invoking a callback when output character data becomes available.
        template<CoreChar CharT, std::invocable<std::basic_string_view<CharT>> Callback, Formattable<CharT>...Values>
        constexpr FormatStatus Write(Callback&& callback, std::basic_string_view<CharT> fmt, const Values&...values)
        {
            Internal::CallbackFormatWriter<CharT, Callback> writer{callback};
            return Format::WriteTo(writer, fmt, values...);
        }

        /// Formats a string by invoking a callback when output character data becomes available.
        template<CoreChar CharT, std::invocable<std::basic_string_view<CharT>> Callback, Formattable<CharT>...Values>
        constexpr FormatStatus Write(Callback&& callback, const CharT* fmt, const Values&...values)
        {
            return Format::Write(std::move(callback), std::basic_string_view<CharT>{fmt}, values...);
        }

        /// Computes the length of a formatted string.
        template<CoreChar CharT, Formattable<CharT>...Values>
        constexpr size_t Length(std::basic_string_view<CharT> fmt, const Values&...values)
        {
            size_t length = 0;
            auto callback = [&length](std::basic_string_view<CharT> str) { length += str.length(); };
            Format::Write(callback, fmt, values...);
            return length;
        }

        /// Formats a string.
        template<CoreChar CharT, Formattable<CharT>...Values>
        std::basic_string<CharT> String(std::basic_string_view<CharT> fmt, const Values&...values)
        {
            std::basic_string<CharT> outStr;
            auto callback = [&outStr](std::basic_string_view<CharT> str) { outStr += str; };
            Format::Write(callback, fmt, values...);
            return outStr;
        }

        /// Formats a string.
        template<CoreChar CharT, Formattable<CharT>...Values>
        std::basic_string<CharT> String(const CharT* fmt, const Values&...values)
        {
            return Format::String<CharT>(std::basic_string_view<CharT>{fmt}, values...);
        }

        /// Formats a string at compile time.
        template<StaticString Fmt, auto...Values>
        consteval auto Static()
        requires (Format::Validate<typename decltype(Fmt)::CharType, decltype(Values)...>(Fmt.GetUnterminatedView()))
        {
            using CharType = typename decltype(Fmt)::CharType;

            constexpr std::basic_string_view<CharType> fmt = Fmt.GetFullView();
            constexpr size_t length = Format::Length(fmt, Values...);

            StaticString<CharType, length> outStr;
            size_t index = 0;
            auto callback = [&outStr, &index](std::basic_string_view<CharType> str) {
                for (CharType ch : str)
                    outStr[index++] = ch;
            };

            Format::Write(callback, fmt, Values...);
            return outStr;
        }

        /// Converts a formattable value to a string.
        template<CoreChar CharT, Formattable<CharT> Value, typename...Args>
        std::basic_string<CharT> ToString(const Value& value, Args&&...args)
        requires (std::is_constructible_v<Formatter<Value, CharT>, decltype(std::forward<Args>(args))...>)
        {
            std::basic_string<CharT> outStr;
            auto callback = [&outStr](std::basic_string_view<CharT> str) { outStr += str; };
            Internal::CallbackFormatWriter<CharT, decltype(callback)> writer{callback};
            Formatter<Value, CharT>{std::forward<Args>(args)...}.Format(value, writer);
            return outStr;
        }

    } // namespace Format

    //==============================================================================================
    // Misc formatters
    //==============================================================================================

    /// Formatter base that only accepts empty `spec` strings. This is used as a placeholder until
    /// more features are needed.
    template<CoreChar CharT>
    struct DumbFormatter {
        constexpr bool Parse(std::basic_string_view<CharT> spec)
        {
            return spec.empty();
        }
    };

    /// Character formatter.
    template<CoreChar CharT>
    struct Formatter<CharT, CharT> : DumbFormatter<CharT> {
        constexpr void Format(CharT ch, FormatWriter<CharT>& writer) const
        {
            writer.Write(ch);
        }
    };

    /// Boolean formatter.
    template<CoreChar CharT>
    struct Formatter<bool, CharT> : DumbFormatter<CharT> {
        constexpr void Format(bool value, FormatWriter<CharT>& writer) const
        {
            for (const char* str = value ? "true" : "false"; *str; ++str)
                writer.Write(CharT(*str));
        }
    };

    //==============================================================================================
    // String formatters
    //==============================================================================================

    /// String view formatter. This is the base of all other string formatters.
    template<CoreChar CharT>
    struct Formatter<std::basic_string_view<CharT>, CharT> : DumbFormatter<CharT> {
        constexpr void Format(std::basic_string_view<CharT> str, FormatWriter<CharT>& writer) const
        {
            writer.Write(str);
        }
    };

    template<CoreChar InChar, CoreChar OutChar>
    struct Formatter<InChar*, OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {};

    template<CoreChar InChar, CoreChar OutChar>
    struct Formatter<const InChar*, OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {};

    template<CoreChar InChar, size_t N, CoreChar OutChar>
    struct Formatter<InChar[N], OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {
        constexpr void Format(const InChar (&str)[N], FormatWriter<OutChar>& writer) const
        {
            Formatter<std::basic_string_view<InChar>, OutChar>::Format(Str::ViewBounded(str), writer);
        }
    };

    template<CoreChar InChar, CoreChar OutChar>
    struct Formatter<std::basic_string<InChar>, OutChar> : Formatter<std::basic_string_view<InChar>, OutChar> {};

    //==============================================================================================
    // Integer formatters
    //==============================================================================================

    struct IntFormatterBase {
        enum class Alignment { Left, Center, Right };
        enum class DigitCase { Lower, Upper };
        enum class SignMode { Normal, Always, Fill };

        static inline constexpr int MinRadix = 2;
        static inline constexpr int MaxRadix = 36;

        Alignment alignment = Alignment::Right;
        DigitCase digitCase = DigitCase::Lower;
        unsigned digitGrouping = 3;
        int radix = 10;
        SignMode signMode = SignMode::Normal;
        unsigned width = 0;
        bool zeroFill = false;
        bool zeroPrefix = false;

        constexpr bool Validate() const
        {
            switch (alignment) {
                case Alignment::Left:
                case Alignment::Center:
                case Alignment::Right:
                    break;
                default:
                    return false;
            }

            if (digitGrouping < 1)
                return false;
            else if (radix < MinRadix || radix > MaxRadix)
                return false;

            switch (signMode) {
                case SignMode::Normal:
                case SignMode::Always:
                case SignMode::Fill:
                    break;
                default:
                    return false;
            }

            return true;
        }
    };

    template<CoreChar CharT>
    struct IntFormatter : IntFormatterBase {
        CharT digitSeparator = 0;
        CharT fillChar = ' ';
        std::basic_string_view<CharT> prefix = {};

        constexpr bool Parse(std::basic_string_view<CharT> spec)
        {
            const CharT* pos = spec.data();
            const CharT* end = pos + spec.length();

            // fillChar
            if (end - pos >= 2) {
                switch (pos[1]) {
                    case '<':
                    case '^':
                    case '>':
                        fillChar = *pos++;
                        break;
                    default:
                        break;
                }
            }

            // alignment
            if (pos != end) {
                switch (*pos) {
                    case '<':
                        this->alignment = IntFormatterBase::Alignment::Left;
                        ++pos;
                        break;
                    case '^':
                        this->alignment = IntFormatterBase::Alignment::Center;
                        ++pos;
                        break;
                    case '>':
                        this->alignment = IntFormatterBase::Alignment::Right;
                        ++pos;
                        break;
                    default:
                        break;
                }
            }

            // signMode
            if (pos != end) {
                switch (*pos) {
                    case '+':
                        this->signMode = IntFormatterBase::SignMode::Always;
                        ++pos;
                        break;
                    case '-':
                        this->signMode = IntFormatterBase::SignMode::Normal;
                        ++pos;
                        break;
                    case ' ':
                        this->signMode = IntFormatterBase::SignMode::Fill;
                        ++pos;
                        break;
                    default:
                        break;
                }
            }

            // usePrefix (the actual prefix is determined later)
            bool usePrefix = false;

            if (pos != end && *pos == '#') {
                usePrefix = true;
                ++pos;
            }

            // zeroFill
            if (pos != end && *pos == '0') {
                this->zeroFill = true;
                ++pos;
            }

            // width
            if (pos != end && Ascii::IsDigit(*pos)) {
                auto result = Parse::Int<unsigned>(pos, end);

                if (result.status != ParseStatus::Ok)
                    return false;

                this->width = result.value;
                pos += result.length;
            }

            // digitSeparator
            if (pos != end && *pos == 'H') {
                digitSeparator = '\'';
                ++pos;

                if (end - pos >= 2 && *pos == ':') {
                    digitSeparator = pos[1];
                    pos += 2;
                }
            }

            // radix and friends
            if (pos != end) {
                switch (*pos) {
                    case 'B':
                    case 'b':
                        this->radix = 2;
                        this->digitGrouping = 4;
                        if (usePrefix) prefix = binary_prefix;
                        ++pos;
                        break;

                    case 'O':
                    case 'o':
                        this->radix = 8;
                        this->digitGrouping = 4;
                        if (usePrefix) this->zeroPrefix = true;
                        ++pos;
                        break;

                    case 'D':
                    case 'I':
                    case 'd':
                    case 'i':
                        ++pos;
                        break;

                    case 'X':
                    case 'x':
                        this->radix = 16;
                        this->digitCase = *pos == 'X' ? IntFormatterBase::DigitCase::Upper : IntFormatterBase::DigitCase::Lower;
                        this->digitGrouping = 4;
                        if (usePrefix) prefix = hexadecimal_prefix;
                        ++pos;
                        break;

                    default:
                        break;
                }
            }

            return pos == end;
        }

    protected:
        constexpr void WriteDigit(int digit, FormatWriter<CharT>& writer) const
        {
            if (digit < 10)
                writer.Write(CharT('0' + digit));
            else if (this->digitCase == IntFormatterBase::DigitCase::Upper)
                writer.Write(CharT('A' - 10 + digit));
            else
                writer.Write(CharT('a' - 10 + digit));
        }

    private:
        static constexpr CharT binary_prefix[] = {'0', 'b', 0};
        static constexpr CharT hexadecimal_prefix[] = {'0', 'x', 0};
    };

    /// Integer formatter.
    template<StrictInt T, CoreChar CharT>
    requires (sizeof(T) >= sizeof(std::int_fast32_t))
    struct Formatter<T, CharT> : IntFormatter<CharT> {
        constexpr void Format(T value, FormatWriter<CharT>& writer) const
        {
            // Determine the sign char to use
            CharT signChar;
            unsigned signLength;

            if (value < 0) {
                signChar = '-';
            } else {
                switch (this->signMode) {
                    case IntFormatterBase::SignMode::Always:
                        signChar = '+';
                        break;
                    case IntFormatterBase::SignMode::Fill:
                        signChar = this->fillChar;
                        break;
                    default:
                        signChar = 0;
                        break;
                }
            }

            signLength = signChar ? 1 : 0;

            // Count digits and separators.
            unsigned nDigitsAndSeparators = CountDigitsAndSeparators(value);

            if (this->zeroFill && signLength + this->prefix.length() + nDigitsAndSeparators < this->width) {
                nDigitsAndSeparators = unsigned(this->width - signLength - this->prefix.length());

                // Never start with a separator!
                if (this->digitSeparator && nDigitsAndSeparators % (this->digitGrouping + 1) == 0)
                    --nDigitsAndSeparators;
            }

            // Fill if the minimum width is not reached.
            unsigned fill;
            unsigned fillLeft = 0;
            unsigned fillRight = 0;

            if (this->width > signLength + this->prefix.length() + nDigitsAndSeparators) {
                fill = unsigned(this->width - signLength - this->prefix.length() - nDigitsAndSeparators);

                switch (this->alignment) {
                    case IntFormatterBase::Alignment::Left:
                        fillRight = fill;
                        break;
                    case IntFormatterBase::Alignment::Center:
                        fillRight = (fill + signLength) / 2;
                        fillLeft = fill - fillRight;
                        break;
                    default:
                        fillLeft = fill;
                        break;
                }
            }

            // Emit output
            writer.Write(fillLeft, this->fillChar);
            if (signChar) writer.Write(signChar);
            writer.Write(this->prefix);
            WriteDigitsAndSeparators(value, writer, 0, nDigitsAndSeparators - 1);
            writer.Write(fillRight, this->fillChar);
        }

    private:
        constexpr unsigned CountDigitsAndSeparators(T value) const
        {
            bool isZero = value == 0;
            unsigned count = 1;

            // Count digits.
            if constexpr(std::is_signed_v<T>) {
                if (value <= T(-this->radix)) {
                    value /= T(-this->radix);
                    ++count;
                }
            }

            while (value >= T(this->radix)) {
                value /= T(this->radix);
                ++count;
            }

            // Include the zero prefix.
            if (this->zeroPrefix && !isZero)
                ++count;

            // Count digit separators.
            if (this->digitSeparator)
                count += (count - 1) / this->digitGrouping;

            return count;
        }

        constexpr void WriteDigitsAndSeparators(T value, FormatWriter<CharT>& writer,
                                                unsigned depth, unsigned remaining) const
        {
            if (this->digitSeparator && depth % (this->digitGrouping + 1) == this->digitGrouping) {
                WriteDigitsAndSeparators(value, writer, depth + 1, remaining - 1);
                writer.Write(this->digitSeparator);
            } else if (value < 0) {
                if (remaining) WriteDigitsAndSeparators(value / T(-this->radix), writer, depth + 1, remaining - 1);
                this->WriteDigit(-int(value % T(-this->radix)), writer);
            } else {
                if (remaining) WriteDigitsAndSeparators(value / T(this->radix), writer, depth + 1, remaining - 1);
                this->WriteDigit(int(value % T(this->radix)), writer);
            }
        }
    };

    template<StrictInt T, CoreChar CharT>
    requires (std::is_signed_v<T> && sizeof(T) < sizeof(std::int_fast32_t))
    struct Formatter<T, CharT> : Formatter<std::int_fast32_t, CharT> {};

    template<StrictInt T, CoreChar CharT>
    requires (std::is_unsigned_v<T> && sizeof(T) < sizeof(std::uint_fast32_t))
    struct Formatter<T, CharT> : Formatter<std::uint_fast32_t, CharT> {};

    /// @}

} // namespace Geo

#endif // GEO_CORE_FORMAT_H_INCLUDED
