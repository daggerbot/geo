/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CLIENT_SDL_MAIN_H_INCLUDED
#define GEO_CLIENT_SDL_MAIN_H_INCLUDED

#include <Client/Main.h>
#include <Render/GL/System.h>

#include "Display.h"

union SDL_Event;
struct SDL_WindowEvent;

namespace Geo {

    /// @addtogroup Client
    /// @{

    /// SDL game client.
    class SdlClient final : public Client {
    public:
        void Initialize(const ClientParams& params);
        void Run();
        void ShutDown();

        Display& GetDisplay() override { return m_display; }
        RenderSystem& GetRenderSystem() override { return m_render; }

        void SetState(std::unique_ptr<ClientState>&& state) override;
        void Quit() override { m_quitRequested = true; }

    private:
        SdlDisplay m_display;
        GlRenderSystem m_render;

        std::unique_ptr<ClientState> m_currentState;
        std::unique_ptr<ClientState> m_pendingState;
        bool m_quitRequested = false;

        void HandleStateTransition();

        void HandleEvent(const SDL_Event& event);
        void HandleWindowEvent(const SDL_WindowEvent& event);
    };

    /// @}

} // namespace Geo

#endif // GEO_CLIENT_SDL_MAIN_H_INCLUDED
