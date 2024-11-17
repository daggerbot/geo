/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <core/strings.h>

#include "cmdline.h"
#include "debug.h"

using namespace geo;

//==================================================================================================
// CommandLineContext
//==================================================================================================

CommandLineContext::CommandLineContext(CommandLineParser& parser, State state)
    : parser_{parser}
    , state_{state}
    , first_arg_{parser.args_[parser_.arg_index_]}
{
    if (state == State::long_option) {
        if (const OsChar* separator = str::find(first_arg_, OsChar('='))) {
            param_ = separator + 1;
            param_explicit_ = true;
        }
    }
}

void CommandLineContext::invalid() const
{
    switch (state_) {
        case State::short_option:
            if (first_arg_[2]) {
                // Note: This will give incorrect output if a parameter has been extracted.
                FATAL("Invalid option: -{} ({})", first_arg_[parser_.char_index_ - 1], first_arg_);
            } else {
                FATAL("Invalid option: {}", first_arg_);
            }

        case State::long_option:
            FATAL("Invalid option: {}", first_arg_);

        default:
            FATAL("Unexpected operand: {}", first_arg_);
    }
}

const OsChar* CommandLineContext::require_param()
{
    extract_param();

    if (param_)
        return param_;

    switch (state_) {
        case State::short_option:
            if (first_arg_[2])
                FATAL("Missing parameter: -{} ({})", first_arg_[parser_.char_index_ - 1], first_arg_);
            else
                FATAL("Missing parameter: {}", first_arg_);

        case State::long_option:
            FATAL("Missing parameter: {}", first_arg_);

        default:
            FATAL("Invalid argument: {}", first_arg_);
    }
}

const OsChar* CommandLineContext::extract_param()
{
    if (param_) {
        param_used_ = true;
        return param_;
    }

    switch (state_) {
        case State::short_option:
            if (first_arg_[parser_.arg_index_]) {
                // Consume the remainder of the current arg, e.g., `-Oparam`.
                param_ = &first_arg_[parser_.arg_index_];
                parser_.char_index_ += str::length(param_);
            } else if (parser_.arg_index_ + 1 < parser_.args_.size() && parser_.args_[parser_.arg_index_ + 1]) {
                // Consume the following arg, e.g., `-O param`.
                param_ = parser_.args_[++parser_.arg_index_];
                parser_.char_index_ = str::length(param_);
            }
            break;

        case State::long_option:
            // If a parameter was specified explicitly, e.g., `--option=param`, then it will already
            // have been extracted from the constructor.
            if (parser_.arg_index_ + 1 < parser_.args_.size() && parser_.args_[parser_.arg_index_ + 1]) {
                // Consume the following arg, e.g., `--option param`.
                param_ = parser_.args_[++parser_.arg_index_];
                parser_.char_index_ = str::length(param_);
            }
            break;

        default:
            return nullptr;
    }

    if (param_)
        param_used_ = true;

    return param_;
}

//==================================================================================================
// CommandLineParser
//==================================================================================================

CommandLineParser::CommandLineParser(int argc, const OsChar* const argv[])
{
    if (argc > 0 && argv && argv[0]) {
        args_ = {argv, size_t(argc)};
        arg_index_ = 1;
    }
}

bool CommandLineParser::is_finished() const
{
    return arg_index_ >= args_.size() || !args_[arg_index_];
}

bool CommandLineParser::parse(CommandLineHandler& handler)
{
    while (!is_finished())
        if (!parse_next(handler))
            return false;

    return true;
}

bool CommandLineParser::parse_next(CommandLineHandler& handler)
{
    bool result;

    if (char_index_) {
        // Handle the next short option in a chain, e.g., `-abc`.
        OsChar opt = args_[arg_index_][char_index_++];
        CommandLineContext context{*this, CommandLineContext::State::short_option};

        result = handler.handle_short_option(opt, context);
    } else if (args_[arg_index_][0] == '-' && args_[arg_index_][1] && !operands_only_) {
        if (args_[arg_index_][1] == '-') {
            if (args_[arg_index_][2]) {
                // Handle a long option, e.g., `--option`.
                OsStringView opt = &args_[arg_index_][2];
                CommandLineContext context{*this, CommandLineContext::State::long_option};

                char_index_ = 2 + opt.length();

                if (context.param_) {
                    // Exclude the parameter from `opt`, e.g., `option`, not `option=param`.
                    opt = opt.substr(0, size_t(context.param_ - opt.data() - 1));
                }

                result = handler.handle_long_option(opt, context);

                if (context.param_explicit_ && !context.param_used_) {
                    // If the param was never used, it probably wasn't expected.
                    FATAL("Unexpected parameter: {}", args_[arg_index_]);
                }
            } else {
                // All args following `--` are treated as positional args.
                operands_only_ = true;
                char_index_ = 2;
                result = true;
            }
        } else {
            // Handle a short option, e.g., `-o`.
            OsChar opt = args_[arg_index_][1];
            CommandLineContext context{*this, CommandLineContext::State::short_option};

            char_index_ = 2;
            result = handler.handle_short_option(opt, context);
        }
    } else {
        // Handle a positional argument.
        const OsChar* arg = args_[arg_index_];
        CommandLineContext context{*this, CommandLineContext::State::operand};

        char_index_ = str::length(arg);
        result = handler.handle_operand(arg, context);
    }

    // Advance if we've reached the end of an arg.
    if (!args_[arg_index_][char_index_]) {
        ++arg_index_;
        char_index_ = 0;
    }

    return result;
}
