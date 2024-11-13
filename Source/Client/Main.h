/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CLIENT_MAIN_H_INCLUDED
#define GEO_CLIENT_MAIN_H_INCLUDED

#include <memory>

#include <Utility/CommandLine.h>

namespace Geo {

    class ClientState;
    class Display;
    class RenderSystem;

    /// @addtogroup Client
    /// @{

    /// Encapsulates most of the game client's global state.
    class Client {
    public:
        virtual ~Client() = 0;

        Client& operator=(const Client&) = delete;

        virtual Display& GetDisplay() = 0;
        virtual RenderSystem& GetRenderSystem() = 0;

        /// Changes the game state. The transition results in the current state being deleted, so it
        /// is deferred until the current event is finished processing.
        virtual void SetState(std::unique_ptr<ClientState>&& state) = 0;

        /// Stops the main loop, resulting in a clean exit.
        virtual void Quit() = 0;
    };

    inline Client::~Client() = default;

    /// Client initialization parameters.
    struct ClientParams : CommandLineHandler {
        const oschar_t* pakPath = nullptr;

        bool HandleShortOption(oschar_t option, CommandLineContext& context) override;
        bool HandleLongOption(OsStringView option, CommandLineContext& context) override;
        bool HandleOperand(const oschar_t* operand, CommandLineContext& context) override;
    };

    /// Client main loop event handler interface.
    class ClientState {
    public:
        virtual ~ClientState() = 0;

        ClientState& operator=(const ClientState&) = delete;

        virtual void BeginState(Client& client);
        virtual void EndState(Client& client);

        virtual void Update(Client& client, u32 deltaMs);
        virtual void Render(Client& client, RenderSystem& render, u32 deltaMs);

        virtual void OnQuit(Client& client);
    };

    /// @}

} // namespace Geo

#endif // GEO_CLIENT_MAIN_H_INCLUDED
