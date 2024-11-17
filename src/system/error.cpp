/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "encoding.h"
#include "error.h"

using namespace geo;

void Error::clear()
{
    description.clear();
    code.clear();
    cause.reset();
}

bool Error::is_empty() const
{
    return description.empty() && !code && !cause;
}

bool Error::matches(const std::error_code& code) const
{
    if (this->code == code)
        return true;
    else if (cause)
        return cause->matches(code);
    else
        return false;
}

bool Error::matches(const std::error_condition& cond) const
{
    if (this->code == cond)
        return true;
    else if (cause)
        return cause->matches(cond);
    else
        return false;
}

fmt::format_context::iterator Error::write_to(fmt::format_context& ctx) const
{
    auto out = ctx.out();
    bool anything_written = false;

    if (is_empty())
        return fmt::format_to(out, "No error");

    if (!description.empty()) {
        fmt::format_to(out, "{}", description);
        anything_written = true;
    }

    if (code) {
        auto code_msg = code.message();

        if (anything_written)
            *out++ = ' ';
        if (!code_msg.empty())
            fmt::format_to(out, "{} ", code_msg);

        fmt::format_to(out, " [{}: {}]", code.category().name(), code.value());
        anything_written = true;
    }

    if (cause && !cause->is_empty()) {
        if (anything_written)
            fmt::format_to(out, " (Caused by: ");

        cause->write_to(ctx);

        if (anything_written)
            *out++ = ')';
    }

    return out;
}

#ifdef _WIN32
fmt::wformat_context::iterator Error::write_to(fmt::wformat_context& ctx) const
{
    return fmt::format_to(ctx.out(), L"{}", encoding::system_to_wide(to_string(*this)));
}
#endif // defined(_WIN32)

std::string geo::to_string(const Error& error)
{
    return fmt::format("{}", error);
}
