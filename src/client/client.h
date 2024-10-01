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

namespace geo {

    class ClientState;

    /// @addtogroup geo_client
    /// @{

    /// Manages the logical state of the game client.
    class Client {
    public:
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;

        /// Returns a pointer to the current client state.
        ClientState* current_state() { return m_currentState.get(); }

        /// Returns the global client instance.
        static Client& get();

        /// Returns true if the quit signal has been sent to the client.
        bool is_quitting() const { return m_quitRequested; }

        /// Runs the game's main loop.
        void main_loop();

        /// Signals the main loop to terminate.
        void quit() { m_quitRequested = true; }

        /// Sets the game state to change to. The change is actually deferred to a time in which it
        /// is safe to do so.
        void set_state(std::unique_ptr<ClientState>&& state);

        /// Switches to the pending client state. This must not be called when processing an event,
        /// as it may result in the current state being deleted.
        void update_state();

    private:
        bool m_quitRequested = false;
        std::unique_ptr<ClientState> m_currentState;
        std::unique_ptr<ClientState> m_pendingState;

        Client();
        ~Client();
    };

    /// Game client event handler.
    class ClientState {
    public:
        virtual ~ClientState() = 0;

        ClientState& operator=(const ClientState&) = delete;

        /// Invoked just after this state is made current.
        virtual void begin_state();

        /// Invoked just before another state is made current.
        virtual void end_state();

        /// Invoked just before the game quits.
        virtual void on_quit();

        /// Renders a frame.
        virtual void render(u32 delta_ms);

        /// Updates the game state just before rendering.
        virtual void update(u32 delta_ms);
    };

    /// @}

} // namespace geo

#endif // GEO_CLIENT_CLIENT_H_INCLUDED
