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
            m_currentState->end_state(*this);
            if (m_quitRequested)
                break;
        }
        m_currentState = std::move(m_pendingState);
        m_currentState->begin_state(*this);
    }
}

//==================================================================================================
// ClientState

ClientState::~ClientState()
{
}

void ClientState::begin_state(Client&)
{
}

void ClientState::end_state(Client&)
{
}

void ClientState::on_quit(Client&)
{
}

void ClientState::render(Client& client, u32)
{
    client.render()->clear();
}

void ClientState::update(Client&, u32)
{
}
