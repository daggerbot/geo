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

#include <core/str.h>
#include <io/stream.h>
#include <render/render.h>
#ifdef _WIN32
# include <system/windows/win32.h>
#endif
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
// Command line
//==================================================================================================

namespace {

    struct ClientParams {
        const oschar_t* assets_path = nullptr;
    };

    struct Option {
        OsStringView opt = {};
        bool expects_param = false;
        void (*callback)() = nullptr;
    };

    LogLevel parse_log_level(OsStringView str)
    {
        if (str == OSSTR "off")
            return LogLevel::none;
        else if (str == OSSTR "fatal")
            return LogLevel::fatal;
        else if (str == OSSTR "error")
            return LogLevel::error;
        else if (str == OSSTR "warning")
            return LogLevel::warning;
        else if (str == OSSTR "info")
            return LogLevel::info;
        else if (str == OSSTR "debug")
            return LogLevel::debug;
        else if (str == OSSTR "trace")
            return LogLevel::trace;
        else
            FATAL("Invalid log level: {}", str);
    }

    ClientParams client_params = {};
    const oschar_t* opt_param = nullptr;

    const Option command_line_options[] = {
        {OSSTR "assets", true, [] { client_params.assets_path = opt_param; }},
        {OSSTR "console", false, [] { debug::enable_console(); }},
        {OSSTR "log-level", true, [] { debug::set_max_log_level(parse_log_level(opt_param)); }},
    };

    const Option& find_option(OsStringView opt)
    {
        for (const Option& option : command_line_options)
            if (option.opt == opt)
                return option;

        FATAL("Invalid option: --{}", opt);
    }

    void handle_command_line(int argc, const oschar_t* const argv[])
    {
        if (argc < 1 || !argv || !argv[0])
            return;

        for (int i = 1; i < argc; ++i) {
            if (!argv[i])
                break;

            // Only accept long options, e.g., `--option`.
            if (argv[i][0] != '-' || argv[i][1] == 0) {
                FATAL("Unexpected argument: {}", argv[i]);
            } else if (argv[i][1] != '-') {
                if (argv[i][2])
                    FATAL("Invalid option: -{} ({})", argv[i][1], argv[i]);
                else
                    FATAL("Invalid option: -{}", argv[i][1]);
            } else if (argv[i][2] == 0) {
                FATAL("Unexpected argument: {}", argv[i]);
            }

            // Get the option string.
            OsStringView opt = &argv[i][2];
            const oschar_t* equals_pos = str::find(opt.data(), '=');

            if (equals_pos) {
                opt = opt.substr(0, size_t(equals_pos - opt.data()));
                opt_param = equals_pos + 1;
            } else {
                opt_param = nullptr;
            }

            // Find the option entry.
            const Option& option = find_option(opt);

            if (option.expects_param) {
                if (!opt_param) {
                    if (i + 1 < argc && argv[i + 1])
                        opt_param = argv[++i];
                    else
                        FATAL("Missing parameter: {}", argv[i]);
                }
            } else if (opt_param) {
                FATAL("Unexpected parameter: {}", argv[i]);
            }

            // Handle the option.
            option.callback();
        }
    }

} // namespace

//==================================================================================================
// Main loop
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
            render::present();
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
// Entry point
//==================================================================================================

namespace {

    int client_main(int argc, const oschar_t* const argv[])
    {
        debug::init_logger();
        handle_command_line(argc, argv);

        LOG_INFO("Initializing...");
        display::init();
        auto pak = system::open_pak(client_params.assets_path);
        render::init(*pak);
        client::set_state(std::make_unique<Playground>());

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
