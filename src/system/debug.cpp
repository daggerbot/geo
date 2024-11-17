/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <cstdlib>

#include <math/math.h>

#include "debug.h"

using namespace geo;

LogLevel debug::detail::max_log_level = LogLevel(LOG_LEVEL_DEFAULT);

void debug::set_max_log_level(LogLevel level)
{
    detail::max_log_level = math::clamp(level, LogLevel::none, LogLevel(LOG_LEVEL_MAX));
}

std::optional<LogLevel> debug::parse_log_level(OsStringView str)
{
    if (str == OSSTR "none")
        return LogLevel::none;
    else if (str == OSSTR "fatal")
        return LogLevel::fatal;
    else if (str == OSSTR "error")
        return LogLevel::error;
    else if (str == OSSTR "warning")
        return LogLevel::warning;
    else if (str == OSSTR "info")
        return LogLevel::info;
    else if (str == OSSTR "debug")
        return LogLevel::debug;
    else if (str == OSSTR "trace")
        return LogLevel::trace;
    else
        return {};
}

void debug::detail::exit_fatal()
{
    std::exit(EXIT_FAILURE);
}