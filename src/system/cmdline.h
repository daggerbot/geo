/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_CMDLINE_H_INCLUDED
#define SYSTEM_CMDLINE_H_INCLUDED

#include <span>

#include <core/strings.h>

namespace geo {

    class CommandLineContext;
    class CommandLineHandler;
    class CommandLineParser;

    /// Additional context used when handling a command line argument.
    class CommandLineContext {
        friend class CommandLineParser;

    public:
        CommandLineContext() = delete;
        CommandLineContext(const CommandLineContext&) = delete;
        CommandLineContext& operator=(const CommandLineContext&) = delete;

        /// Shows an error message indicating that the argument is invalid, and exits with an error
        /// code.
        [[noreturn]] void invalid() const;

        /// Gets the option parameter. Shows an error message and exits with an error code if no
        /// parameter is available.
        const OsChar* require_param();

    private:
        enum class State { short_option, long_option, operand };

        CommandLineParser& parser_;
        State state_;
        const OsChar* first_arg_ = nullptr;
        const OsChar* param_ = nullptr;
        bool param_explicit_ = false;
        bool param_used_ = false;

        explicit CommandLineContext(CommandLineParser& parser, State state);

        const OsChar* extract_param();
    };

    /// Interface for handling command line arguments.
    class CommandLineHandler {
    public:
        virtual ~CommandLineHandler() = 0;

        CommandLineHandler& operator=(const CommandLineHandler&) = delete;

        /// Handles a short option, e.g., `-o`.
        ///
        /// @return True to continue parsing, or false to stop parsing. Note that returning false
        ///   does not necessarily indicate an error.
        virtual bool handle_short_option(OsChar opt, CommandLineContext& context) = 0;

        /// Handles a long option, e.g., `--option`.
        ///
        /// @return True to continue parsing, or false to stop parsing. Note that returning false
        ///   does not necessarily indicate an error.
        virtual bool handle_long_option(OsStringView opt, CommandLineContext& context) = 0;

        /// Handles a positional argument.
        ///
        /// @return True to continue parsing, or false to stop parsing. Note that returning false
        ///   does not necessarily indicate an error.
        virtual bool handle_operand(const OsChar* arg, CommandLineContext& context) = 0;
    };

    inline CommandLineHandler::~CommandLineHandler() = default;

    /// Parses the command line.
    class CommandLineParser {
        friend class CommandLineContext;

    public:
        CommandLineParser() = default;
        CommandLineParser(const CommandLineParser& other) = default;
        explicit CommandLineParser(int argc, const OsChar* const argv[]);

        CommandLineParser& operator=(const CommandLineParser& other) = default;

        /// Indicates whether the end of the command line has been reached.
        bool is_finished() const;

        /// Continues parsing the command line until the end is reached or one of the `handler`
        /// functions returns false.
        ///
        /// @return True if the end of the command line was reached, or false if one of the
        ///   `handler` functions returned false.
        bool parse(CommandLineHandler& handler);

    private:
        std::span<const OsChar* const> args_ = {};
        size_t arg_index_ = 0;
        size_t char_index_ = 0;
        bool operands_only_ = false;

        bool parse_next(CommandLineHandler& handler);
    };

} // namespace geo

#endif // SYSTEM_CMDLINE_H_INCLUDED
