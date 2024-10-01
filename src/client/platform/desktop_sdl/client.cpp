/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <SDL_events.h>
#include <SDL_timer.h>

#include <geo/system/debug.h>

#include "../../client.h"
#include "../../display.h"

using namespace geo;

namespace {

    void handle_window_event(Client& client, const SDL_WindowEvent& event)
    {
        switch (event.event) {
        case SDL_WINDOWEVENT_CLOSE:
            client.quit();
            break;

        default:
            break;
        }
    }

    void handle_event(Client& client, const SDL_Event& event)
    {
        switch (event.type) {
        case SDL_QUIT:
            client.quit();
            break;

        case SDL_WINDOWEVENT:
            ::handle_window_event(client, event.window);
            break;

        default:
            break;
        }
    }

    void handle_pending_events(Client& client)
    {
        SDL_Event event;

        while (!client.is_quitting() && ::SDL_PollEvent(&event)) {
            ::handle_event(client, event);
            client.update_state();
        }
    }

} // namespace

void Client::main_loop()
{
    u32 current_ms;
    u32 prev_ms = ::SDL_GetTicks();
    u32 delta_ms;

    Display& display = Display::get();

    update_state();
    DASSERT(m_currentState != nullptr);

    while (!m_quitRequested) {
        // Handle window and input events.
        ::handle_pending_events(*this);
        if (m_quitRequested)
            break;

        // Update the game time.
        current_ms = ::SDL_GetTicks();
        delta_ms = current_ms - prev_ms;
        prev_ms = current_ms;

        // Update the game state.
        m_currentState->update(delta_ms);
        update_state();
        if (m_quitRequested)
            break;

        // Render the scene.
        m_currentState->render(delta_ms);
        update_state();
        display.swap_buffers();
    }

    m_currentState->on_quit();
}
