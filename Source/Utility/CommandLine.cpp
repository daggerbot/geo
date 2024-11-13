/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <Core/Str.h>
#include <System/Debug.h>

#include "CommandLine.h"

using namespace Geo;

//==================================================================================================
// CommandLineContext
//==================================================================================================

CommandLineContext::CommandLineContext(CommandLineParser& parser, State state)
    : m_parser{parser}
    , m_state{state}
{
    // Extract an explicit parameter now if present.
    if (state == State::LongOption) {
        auto separator = Str::Find(parser.m_args[parser.m_argIndex], oschar_t('='));
        if (separator)
            m_param = separator + 1;
    }
}

void CommandLineContext::ExpectNoParam() const
{
    if (m_param)
        FATAL("Unexpected parameter: {}", m_parser.m_args[m_parser.m_argIndex]);
}

const oschar_t* CommandLineContext::ExpectParam()
{
    if (m_param)
        return m_param;

    switch (m_state) {
        case State::ShortOption:
            if (m_parser.m_args[m_parser.m_argIndex][m_parser.m_charIndex] != 0) {
                // Consume the rest of the current argument (e.g. `-Oparam`).
                m_param = &m_parser.m_args[m_parser.m_argIndex][m_parser.m_charIndex];
                m_parser.m_charIndex += Str::Length(m_param);
                return m_param;
            } else if (m_parser.m_argIndex + 1 < m_parser.m_args.size() && m_parser.m_args[m_parser.m_argIndex + 1]) {
                // Consume the following argument (e.g. `-O param`).
                m_param = m_parser.m_args[++m_parser.m_argIndex];
                m_parser.m_charIndex = Str::Length(m_param);
                return m_param;
            } else {
                if (m_parser.m_charIndex == 2)
                    FATAL("Missing parameter: {}", m_parser.m_args[m_parser.m_argIndex]);
                else
                    FATAL("Missing parameter: -{} (in {})",
                          m_parser.m_args[m_parser.m_argIndex][m_parser.m_charIndex - 1],
                          m_parser.m_args[m_parser.m_argIndex]);
            }

        case State::LongOption:
            if (m_parser.m_argIndex + 1 < m_parser.m_args.size() && m_parser.m_args[m_parser.m_argIndex + 1]) {
                // Consume the following argument (e.g. `--option param`).
                m_param = m_parser.m_args[++m_parser.m_argIndex];
                m_parser.m_charIndex = Str::Length(m_param);
                return m_param;
            } else {
                FATAL("Missing parameter: {}", m_parser.m_args[m_parser.m_argIndex]);
            }

        default:
            return nullptr;
    }
}

void CommandLineContext::Invalid() const
{
    switch (m_state) {
        case State::ShortOption:
            if (m_parser.m_charIndex == 2 && !m_parser.m_args[m_parser.m_argIndex][m_parser.m_charIndex])
                FATAL("Invalid option: {}", m_parser.m_args[m_parser.m_argIndex]);
            else
                FATAL("Invalid option: -{} (in {})",
                      m_parser.m_args[m_parser.m_argIndex][m_parser.m_charIndex - 1],
                      m_parser.m_args[m_parser.m_argIndex]);

        case State::LongOption:
            FATAL("Invalid option: {}", m_parser.m_args[m_parser.m_argIndex]);

        case State::Operand:
            FATAL("Unexpected operand: {}", m_parser.m_args[m_parser.m_argIndex]);

        default:
            for (;;); // unreachable
    }
}

//==================================================================================================
// CommandLineParser
//==================================================================================================

CommandLineParser::CommandLineParser(int argc, const oschar_t* const argv[])
{
    if (argc > 0 && argv && argv[0]) {
        m_args = {argv, size_t(argc)};
        m_argIndex = 1;
    }
}

bool CommandLineParser::IsFinished() const
{
    return m_argIndex >= m_args.size() || m_args[m_argIndex] == nullptr;
}

bool CommandLineParser::Parse(CommandLineHandler& handler)
{
    while (!IsFinished())
        if (!ParseNext(handler))
            return false;

    return true;
}

bool CommandLineParser::ParseNext(CommandLineHandler& handler)
{
    bool result;

    if (m_charIndex > 0) {
        // Handle the next short option in a chain (e.g. `-abc`).
        oschar_t option = m_args[m_argIndex][m_charIndex++];
        CommandLineContext context{*this, CommandLineContext::State::ShortOption};

        result = handler.HandleShortOption(option, context);
    } else if (m_args[m_argIndex][0] == '-' && m_args[m_argIndex][1] && !m_onlyOperands) {
        if (m_args[m_argIndex][1] == '-') {
            if (m_args[m_argIndex][2]) {
                // Handle a long option (e.g. `--option`).
                OsStringView option = &m_args[m_argIndex][2];
                CommandLineContext context{*this, CommandLineContext::State::LongOption};

                m_charIndex = 2 + option.length();

                if (context.m_param) {
                    // If a parameter was found, exclude it from `option`.
                    option = option.substr(0, size_t(context.m_param - 1 - option.data()));
                }

                result = handler.HandleLongOption(option, context);
            } else {
                // All arguments after `--` are operands, even if they start with `-`.
                m_onlyOperands = true;
                m_charIndex = 2;
                result = true;
            }
        } else {
            // Handle a short option (e.g. `-o`) or the first option in a short option chain (e.g. `-abc`).
            oschar_t option = m_args[m_argIndex][1];
            CommandLineContext context{*this, CommandLineContext::State::ShortOption};

            m_charIndex = 2;
            result = handler.HandleShortOption(option, context);
        }
    } else {
        // Handle a positional argument.
        const oschar_t* operand = m_args[m_argIndex];
        CommandLineContext context{*this, CommandLineContext::State::Operand};

        m_charIndex = Str::Length(operand);
        result = handler.HandleOperand(operand, context);
    }

    // Advance to the next argument if we've consumed the current one.
    if (m_args[m_argIndex][m_charIndex] == 0) {
        ++m_argIndex;
        m_charIndex = 0;
    }

    return result;
}
