/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CLIENT_MAIN_H_INCLUDED
#define CLIENT_MAIN_H_INCLUDED

#include <memory>

#include <core/types.h>

namespace geo {

    /// Client main loop event handler.
    class ClientState {
    public:
        virtual ~ClientState() = 0;

        ClientState& operator=(const ClientState&) = delete;

        virtual void begin_state();
        virtual void end_state();

        virtual void update(u32 delta_ms);
        virtual void render(u32 delta_ms);

        virtual void on_quit();
    };

    /// Client state management functions.
    namespace client {

        void set_state(std::unique_ptr<ClientState>&& state);
        void quit();

    } // namespace client

} // namespace geo

#endif // CLIENT_MAIN_H_INCLUDED
