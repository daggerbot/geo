/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <SDL_events.h>
#include <SDL_timer.h>

#include <geo/render/system.h>
#include <geo/system/debug.h>

#include "../../client.h"
#include "../../display.h"

using namespace geo;

Client::Client()
{
}

Client::~Client()
{
    shut_down();
}

void Client::initialize()
{
    m_display = std::make_unique<Display>();
    m_display->initialize();

    m_render = std::make_unique<RenderSystem>();
    m_render->initialize(*m_display);
}

void Client::main_loop()
{
    u32 prev_ms = ::SDL_GetTicks();
    u32 current_ms;
    u32 delta_ms;

    // Make sure the initial client state was set.
    update_state();
    DASSERT(m_currentState != nullptr);

    while (!m_quitRequested) {
        // Handle window and input events.
        handle_pending_events();
        if (m_quitRequested)
            break;

        // Update the game time.
        current_ms = ::SDL_GetTicks();
        delta_ms = current_ms - prev_ms;
        prev_ms = current_ms;

        // Update the game state.
        m_currentState->update(*this, delta_ms);
        update_state();
        if (m_quitRequested)
            break;

        // Render the scene.
        m_currentState->render(*this, delta_ms);
        update_state();
        m_display->swap_buffers();
    }

    m_currentState->on_quit(*this);
}

void Client::shut_down()
{
    m_render.reset();
    m_display.reset();
}

void Client::handle_pending_events()
{
    SDL_Event event;

    while (!m_quitRequested && ::SDL_PollEvent(&event)) {
        handle_event(event);
        update_state();
    }
}

void Client::handle_event(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_QUIT:
        quit();
        break;

    case SDL_WINDOWEVENT:
        handle_window_event(event.window);
        break;

    default:
        break;
    }
}

void Client::handle_window_event(const SDL_WindowEvent& event)
{
    switch (event.event) {
    case SDL_WINDOWEVENT_CLOSE:
        quit();
        break;
    default:
        break;
    }
}
