/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <geo/render/system.h>

#include "client.h"

using namespace geo;

//==================================================================================================
// Client

Client::Client()
{
}

Client::~Client()
{
}

Client& Client::get()
{
    static Client instance;
    return instance;
}

void Client::set_state(std::unique_ptr<ClientState>&& state)
{
    if (state)
        m_pendingState = std::move(state);
    else
        quit();
}

void Client::update_state()
{
    while (m_pendingState && !m_quitRequested) {
        if (m_currentState) {
            m_currentState->end_state();
            if (m_quitRequested)
                break;
        }
        m_currentState = std::move(m_pendingState);
        m_currentState->begin_state();
    }
}

//==================================================================================================
// ClientState

ClientState::~ClientState()
{
}

void ClientState::begin_state()
{
}

void ClientState::end_state()
{
}

void ClientState::on_quit()
{
}

void ClientState::render(u32)
{
    RenderSystem::get().clear();
}

void ClientState::update(u32)
{
}
