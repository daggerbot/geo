/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifdef _WIN32
# include <windows.h>
#endif

#include <SDL.h>

#include <Client/Playground.h>
#include <System/Debug.h>
#include <System/System.h>

#include "Main.h"

using namespace Geo;

//==================================================================================================
// SdlClient
//==================================================================================================

void SdlClient::Initialize(const ClientParams& params)
{
    m_display.Initialize();
    auto pak = System::OpenAssetPak(params.pakPath);
    m_render.Initialize(*pak);
}

void SdlClient::Run()
{
    SDL_Event event;
    u32 currentTimeMs;
    u32 prevTimeMs = SDL_GetTicks();
    u32 deltaMs;

    HandleStateTransition();
    ASSERT(m_currentState != nullptr);

    while (!m_quitRequested) {
        // Handle window and input events.
        while (SDL_PollEvent(&event)) {
            HandleEvent(event);
            HandleStateTransition();
            if (m_quitRequested)
                break;
        }

        if (m_quitRequested)
            break;

        // Update the game clock.
        currentTimeMs = SDL_GetTicks();
        deltaMs = currentTimeMs - prevTimeMs;
        prevTimeMs = currentTimeMs;

        // Simulate the frame.
        m_currentState->Update(*this, deltaMs);
        HandleStateTransition();
        if (m_quitRequested)
            break;

        // Render the frame.
        m_render.BeginDraw(m_display);
        m_currentState->Render(*this, m_render, deltaMs);
        m_render.EndDraw();
        m_display.SwapBuffers();
    }
}

void SdlClient::ShutDown()
{
    m_render.ShutDown();
    m_display.ShutDown();
}

void SdlClient::SetState(std::unique_ptr<ClientState>&& state)
{
    ASSERT(state != nullptr);
    m_pendingState = std::move(state);
}

void SdlClient::HandleStateTransition()
{
    while (m_pendingState && !m_quitRequested) {
        if (m_currentState) {
            m_currentState->EndState(*this);
            if (m_quitRequested)
                break;
        }

        m_currentState = std::move(m_pendingState);
        m_currentState->BeginState(*this);
    }
}

void SdlClient::HandleEvent(const SDL_Event& event)
{
    switch (event.type) {
        case SDL_QUIT:
            Quit();
            break;

        case SDL_WINDOWEVENT:
            HandleWindowEvent(event.window);
            break;

        default:
            break;
    }
}

void SdlClient::HandleWindowEvent(const SDL_WindowEvent& event)
{
    switch (event.event) {
        case SDL_WINDOWEVENT_CLOSE:
            Quit();
            break;

        default:
            break;
    }
}

//==================================================================================================
// Main functions
//==================================================================================================

namespace {

    // Main function for all platforms.
    int ClientMain(int argc, const oschar_t* const argv[])
    {
        SdlClient client;

        {
            ClientParams params;
            CommandLineParser{argc, argv}.Parse(params);
            LOG_INFO("Initializing...");
            client.Initialize(params);
        }

        client.SetState(std::make_unique<Playground>());
        LOG_INFO("Game started!");
        client.Run();

        LOG_INFO("Shutting down...");
        client.ShutDown();
        SDL_Quit();
        return 0;
    }

} // namespace

#ifdef _WIN32

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    return ClientMain(__argc, __wargv);
}

#else // !defined(_WIN32)

int main(int argc, char* argv[])
{
    return ClientMain(argc, argv);
}

#endif // !defined(_WIN32)
