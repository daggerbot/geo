/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <SDL_video.h>

#include <geo/core/game_defs.h>
#include <geo/render/gl/version.h>
#include <geo/system/debug.h>

#include "../../display.h"

using namespace geo;

Display::Display()
    : m_window{nullptr, &SDL_DestroyWindow}
    , m_glContext{nullptr, &SDL_GL_DeleteContext}
{
}

Display::~Display()
{
    shut_down();
}

void Display::initialize()
{
    if (!m_window) {
        LOG_TRACE("Creating SDL window");

        ::SDL_GL_ResetAttributes();
        ::SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        ::SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        ::SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        ::SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        ::SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
        ::SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        ::SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        ::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
        ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, RENDER_GL_VERSION_MAJOR);
        ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, RENDER_GL_VERSION_MINOR);
        ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        m_window.reset(::SDL_CreateWindow(
            GAME_TITLE,
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN));

        if (!m_window) {
            FATAL("Can't create SDL window: {}", ::SDL_GetError());
        }
    }

    if (!m_glContext) {
        LOG_TRACE("Creating OpenGL context");

        m_glContext.reset(::SDL_GL_CreateContext(m_window.get()));

        if (!m_glContext) {
            FATAL("Can't create OpenGL context: {}", ::SDL_GetError());
        }

        // Try to enable vsync. Eventually we'll make this configurable.
        if (::SDL_GL_SetSwapInterval(-1) == 0)
            LOG_INFO("Enabled adaptive vsync");
        else if (::SDL_GL_SetSwapInterval(1) == 0)
            LOG_INFO("Enabled standard vsync");
        else
            LOG_WARNING("Can't enabled vsync: {}", ::SDL_GetError());
    }
}

void Display::shut_down()
{
    if (m_glContext) {
        LOG_TRACE("Destroying OpenGL context");
        m_glContext.reset();
    }

    if (m_window) {
        LOG_TRACE("Destroying SDL window");
        m_window.reset();
    }
}

Display& Display::get()
{
    static Display instance;
    return instance;
}

void* Display::gl_get_proc_address(const char* name)
{
    return ::SDL_GL_GetProcAddress(name);
}

Vec2i Display::size() const
{
    Vec2i size = {0, 0};

    ::SDL_GetWindowSize(m_window.get(), &size.x, &size.y);
    return size;
}

void Display::swap_buffers()
{
    ::SDL_GL_SwapWindow(m_window.get());
}
