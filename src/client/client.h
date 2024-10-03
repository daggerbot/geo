/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CLIENT_CLIENT_H_INCLUDED
#define GEO_CLIENT_CLIENT_H_INCLUDED

#include <memory>

#include <geo/core/types.h>

union SDL_Event;
struct SDL_WindowEvent;

namespace geo {

    class ClientState;
    class Display;
    class RenderSystem;

    /// @addtogroup geo_client
    /// @{

    /// Manages the logical state of the game client.
    class Client {
    public:
        Client();
        Client(const Client&) = delete;
        ~Client();

        Client& operator=(const Client&) = delete;

        void initialize();
        void main_loop();
        void shut_down();

        /// Returns a pointer to the render surface.
        Display* display() { return m_display.get(); }

        /// Signals the main loop to terminate.
        void quit() { m_quitRequested = true; }

        /// Returns a pointer to the render system.
        RenderSystem* render() { return m_render.get(); }

        /// Sets the game state to change to. The change is actually deferred to a time in which it
        /// is safe to do so.
        void set_state(std::unique_ptr<ClientState>&& state);

    private:
        std::unique_ptr<Display> m_display;
        std::unique_ptr<RenderSystem> m_render;

        std::unique_ptr<ClientState> m_currentState;
        std::unique_ptr<ClientState> m_pendingState;
        bool m_quitRequested = false;

        /// Switches to the pending client state. This must not be called when processing an event,
        /// as it may result in the current state being deleted.
        void update_state();

        void handle_pending_events();
        void handle_event(const SDL_Event& event);
        void handle_window_event(const SDL_WindowEvent& event);
    };

    /// Game client event handler.
    class ClientState {
    public:
        virtual ~ClientState() = 0;

        ClientState& operator=(const ClientState&) = delete;

        /// Invoked just after this state is made current.
        virtual void begin_state(Client& client);

        /// Invoked just before another state is made current.
        virtual void end_state(Client& client);

        /// Invoked just before the game quits.
        virtual void on_quit(Client& client);

        /// Renders a frame.
        virtual void render(Client& client, u32 delta_ms);

        /// Updates the game state just before rendering.
        virtual void update(Client& client, u32 delta_ms);
    };

    /// @}

} // namespace geo

#endif // GEO_CLIENT_CLIENT_H_INCLUDED
