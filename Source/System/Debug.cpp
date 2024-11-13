/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Debug.h"

using namespace Geo;

std::optional<LogLevel> Debug::ParseLogLevel(OsStringView str)
{
    if (str == OSSTR "off")
        return LogLevel::Off;
    else if (str == OSSTR "fatal")
        return LogLevel::Fatal;
    else if (str == OSSTR "error")
        return LogLevel::Error;
    else if (str == OSSTR "warning")
        return LogLevel::Warning;
    else if (str == OSSTR "info")
        return LogLevel::Info;
    else if (str == OSSTR "debug")
        return LogLevel::Debug;
    else if (str == OSSTR "trace")
        return LogLevel::Trace;
    else
        return {};
}
