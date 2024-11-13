/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <Render/System.h>
#include <System/Debug.h>

#include "Main.h"

using namespace Geo;

//==================================================================================================
// ClientParams
//==================================================================================================

bool ClientParams::HandleShortOption(oschar_t, CommandLineContext& context)
{
    context.Invalid();
}

bool ClientParams::HandleLongOption(OsStringView option, CommandLineContext& context)
{
    if (option == OSSTR "console") {
        Debug::EnableConsole();
        return true;
    } else if (option == OSSTR "log-level") {
        auto level = Debug::ParseLogLevel(context.ExpectParam());

        if (!level)
            FATAL("Invalid log level: {}", context.ExpectParam());

        Debug::SetMaxLogLevel(*level);
        return true;
    } else if (option == OSSTR "pak") {
        pakPath = context.ExpectParam();
        return true;
    } else {
        context.Invalid();
    }
}

bool ClientParams::HandleOperand(const oschar_t*, CommandLineContext& context)
{
    context.Invalid();
}

//==================================================================================================
// ClientState
//==================================================================================================

ClientState::~ClientState()
{
}

void ClientState::BeginState(Client&)
{
}

void ClientState::EndState(Client&)
{
}

void ClientState::Update(Client&, u32)
{
}

void ClientState::Render(Client&, RenderSystem& render, u32)
{
    render.ClearColorBuffer();
}

void ClientState::OnQuit(Client&)
{
}
