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

#include <io/stream.h>
#include <render/render.h>
#ifdef _WIN32
# include <system/windows/win32.h>
#endif
#include <system/cmdline.h>
#include <system/debug.h>
#include <system/system.h>

#include "display.h"
#include "main.h"
#include "playground.h"

using namespace geo;

//==================================================================================================
// ClientState
//==================================================================================================

ClientState::~ClientState()
{
}

void ClientState::begin_state()
{
}

void ClientState::end_state()
{
}

void ClientState::update(u32)
{
}

void ClientState::render(u32)
{
    render::clear_color_buffer();
}

void ClientState::on_quit()
{
}

//==================================================================================================
// ClientParams
//==================================================================================================

namespace {

    struct ClientParams : CommandLineHandler {
        const OsChar* assets_path = nullptr;

        bool handle_short_option(OsChar, CommandLineContext& context) override
        {
            context.invalid();
        }

        bool handle_long_option(OsStringView opt, CommandLineContext& context) override
        {
            if (opt == OSSTR "assets") {
                assets_path = context.require_param();
                return true;
            } else if (opt == OSSTR "console") {
                debug::enable_console();
                return true;
            } else if (opt == OSSTR "log-level") {
                auto param = context.require_param();
                auto level = debug::parse_log_level(param);

                if (!level)
                    FATAL("Invalid --log-level: {}", param);

                debug::set_max_log_level(*level);
                return true;
            } else {
                context.invalid();
            }
        }

        bool handle_operand(const OsChar*, CommandLineContext& context) override
        {
            context.invalid();
        }
    };

} // namespace

//==================================================================================================
// Main loop functions
//==================================================================================================

namespace {

    std::unique_ptr<ClientState> current_state;
    std::unique_ptr<ClientState> pending_state;
    bool quit_requested = false;

    void handle_state_transition()
    {
        while (pending_state && !quit_requested) {
            if (current_state) {
                current_state->end_state();
                if (quit_requested)
                    break;
            }

            current_state = std::move(pending_state);
            current_state->begin_state();
        }
    }

    void handle_window_event(const SDL_WindowEvent& event)
    {
        switch (event.event) {
            case SDL_WINDOWEVENT_CLOSE:
                quit_requested = true;
                break;

            default:
                break;
        }
    }

    void handle_event(const SDL_Event& event)
    {
        switch (event.type) {
            case SDL_QUIT:
                quit_requested = true;
                break;

            case SDL_WINDOWEVENT:
                handle_window_event(event.window);
                break;

            default:
                break;
        }
    }

    void main_loop()
    {
        SDL_Event event;
        u32 current_time_ms;
        u32 prev_time_ms = SDL_GetTicks();
        u32 delta_ms;

        handle_state_transition();
        ASSERT(current_state != nullptr);

        while (!quit_requested) {
            // Handle window and input events.
            while (SDL_PollEvent(&event)) {
                handle_event(event);
                handle_state_transition();
                if (quit_requested)
                    break;
            }

            if (quit_requested)
                break;

            // Update the game clock.
            current_time_ms = SDL_GetTicks();
            delta_ms = current_time_ms - prev_time_ms;
            prev_time_ms = current_time_ms;

            // Simulate the frame's game logic.
            current_state->update(delta_ms);
            handle_state_transition();
            if (quit_requested)
                break;

            // Render the scene.
            render::begin_draw();
            current_state->render(delta_ms);
            render::end_draw();
            display::swap_buffers();
        }

        current_state->on_quit();
    }

} // namespace

void client::set_state(std::unique_ptr<ClientState>&& state)
{
    ASSERT(state != nullptr);
    pending_state = std::move(state);
}

void client::quit()
{
    quit_requested = true;
}

//==================================================================================================
// Entry point functions
//==================================================================================================

namespace {

    int client_main(int argc, const OsChar* const argv[])
    {
        debug::init_logger();

        {
            ClientParams params;

            CommandLineParser{argc, argv}.parse(params);

            LOG_INFO("Initializing...");
            display::init();
            auto pak = system::open_pak(params.assets_path);
            render::init(*pak);
            client::set_state(std::make_unique<Playground>());
        }

        LOG_INFO("Game started!");
        main_loop();

        LOG_INFO("Shutting down...");
        render::shut_down();
        display::shut_down();
        SDL_Quit();
        debug::shut_down_logger();

        return 0;
    }

} // namespace

#ifdef _WIN32

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    win32::set_message_box_proc(&MessageBoxW);
    return client_main(__argc, __wargv);
}

#else // !defined(_WIN32)

int main(int argc, char* argv[])
{
    return client_main(argc, argv);
}

#endif // !defined(_WIN32)
