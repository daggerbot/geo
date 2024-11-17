/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <SDL_video.h>

#include <core/game_defs.h>
#include <system/debug.h>

#include "display.h"

using namespace geo;

namespace {

    SDL_Window* sdl_window = nullptr;
    SDL_GLContext gl_context = nullptr;

} // namespace

void display::init()
{
    LOG_INFO("Creating SDL window");

    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl::major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl::minor_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    sdl_window = SDL_CreateWindow(
        GAME_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    if (!sdl_window)
        FATAL("Can't create SDL window: {}", SDL_GetError());

    LOG_INFO("Creating OpenGL context");

    gl_context = SDL_GL_CreateContext(sdl_window);

    if (!gl_context)
        FATAL("Can't create OpenGL context: {}", SDL_GetError());

    // Enable vsync. Eventually we'll make this configurable.
    if (!SDL_GL_SetSwapInterval(-1))
        LOG_INFO("Enabled adaptive vsync");
    else if (!SDL_GL_SetSwapInterval(1))
        LOG_INFO("Enabled vsync");
    else
        LOG_WARNING("Can't enable vsync: {}", SDL_GetError());
}

void display::shut_down()
{
    SDL_GL_MakeCurrent(nullptr, nullptr);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);

    sdl_window = nullptr;
    gl_context = nullptr;
}

Vec2i display::get_window_size()
{
    Vec2i size = {0, 0};

    SDL_GetWindowSize(sdl_window, &size.x, &size.y);
    return size;
}

void display::swap_buffers()
{
    SDL_GL_SwapWindow(sdl_window);
}

void* display::gl_get_proc_address(const char* name)
{
    return SDL_GL_GetProcAddress(name);
}
