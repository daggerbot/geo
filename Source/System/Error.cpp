/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <Core/Format.h>

#include "Error.h"

using namespace Geo;

void Error::Clear()
{
    description.clear();
    code.clear();
    innerError.reset();
}

std::string Error::GetFullMessage() const
{
    return Format::ToString<char>(*this);
}

bool Error::IsEmpty() const
{
    return description.empty() && !code && !innerError;
}

void Error::WriteTo(FormatWriter<char>& writer) const
{
    bool anythingWritten = false;

    if (IsEmpty()) {
        writer.Write("No error");
        return;
    }

    if (!description.empty()) {
        writer.Write(description);
        anythingWritten = true;
    }

    if (code) {
        auto codeMsg = code.message();

        if (anythingWritten)
            writer.Write(": ");

        if (!codeMsg.empty()) {
            writer.Write(codeMsg);
            writer.Write(' ');
        }

        Format::WriteTo(writer, "[{}: {}]", code.category().name(), code.value());
        anythingWritten = true;
    }

    if (innerError && *innerError) {
        if (anythingWritten)
            writer.Write(" (Caused by: ");

        innerError->WriteTo(writer);

        if (anythingWritten)
            writer.Write(')');
    }
}
