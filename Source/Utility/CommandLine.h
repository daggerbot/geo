/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_UTILITY_COMMANDLINE_H_INCLUDED
#define GEO_UTILITY_COMMANDLINE_H_INCLUDED

#include <span>

#include <Core/StringTypes.h>

namespace Geo {

    /// @addtogroup Utility
    /// @{

    class CommandLineContext;
    class CommandLineHandler;
    class CommandLineParser;

    /// Provides additional contextual information when handling command line arguments.
    class CommandLineContext {
        friend class CommandLineParser;

    public:
        CommandLineContext() = delete;
        CommandLineContext(const CommandLineContext&) = delete;

        CommandLineContext& operator=(const CommandLineContext&) = delete;

        /// Raises a fatal error if an option parameter was explicitly specified when not expected.
        void ExpectNoParam() const;

        /// Extracts and returns the option parameter. Fatal if no parameter is available.
        const oschar_t* ExpectParam();

        /// Raises a fatal error indicating that the argument is invalid.
        [[noreturn]] void Invalid() const;

    private:
        enum class State { ShortOption, LongOption, Operand };

        CommandLineParser& m_parser;
        State m_state;
        const oschar_t* m_param = nullptr;

        explicit CommandLineContext(CommandLineParser& parser, State state);
    };

    /// Interface for handling command line arguments.
    class CommandLineHandler {
    public:
        virtual ~CommandLineHandler() = 0;

        CommandLineHandler& operator=(const CommandLineHandler&) = delete;

        /// Handles a short option, e.g. `-o`.
        ///
        /// @return `true` to continue parsing, or `false` to stop. Returning false does not
        ///   necessarily indicate an error.
        virtual bool HandleShortOption(oschar_t option, CommandLineContext& context) = 0;

        /// Handles a long option, e.g. `--option`.
        ///
        /// @return `true` to continue parsing, or `false` to stop. Returning false does not
        ///   necessarily indicate an error.
        virtual bool HandleLongOption(OsStringView option, CommandLineContext& context) = 0;

        /// Handles a positional argument.
        ///
        /// @return `true` to continue parsing, or `false` to stop. Returning false does not
        ///   necessarily indicate an error.
        virtual bool HandleOperand(const oschar_t* operand, CommandLineContext& context) = 0;
    };

    inline CommandLineHandler::~CommandLineHandler() = default;

    /// Parses the command line.
    class CommandLineParser {
        friend class CommandLineContext;

    public:
        CommandLineParser() = default;
        CommandLineParser(const CommandLineParser& other) = default;
        explicit CommandLineParser(int argc, const oschar_t* const argv[]);

        CommandLineParser& operator=(const CommandLineParser& other) = default;

        /// Indicates whether parsing has finished.
        bool IsFinished() const;

        /// Begins or continues parsing the command line.
        ///
        /// @return `true` if parsing completed, or `false` if parsing stopped due to one of the
        ///   `handler` functions returning false. Note that a return value of false does not
        ///   necessarily indicate an eror.
        bool Parse(CommandLineHandler& handler);

    private:
        std::span<const oschar_t* const> m_args = {};
        size_t m_argIndex = 0;
        size_t m_charIndex = 0;
        bool m_onlyOperands = false;

        bool ParseNext(CommandLineHandler& handler);
    };

    /// @}

} // namespace Geo

#endif // GEO_UTILITY_COMMANDLINE_H_INCLUDED
