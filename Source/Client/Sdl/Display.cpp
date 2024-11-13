/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <SDL_video.h>

#include <Core/GameDefs.h>
#include <System/Debug.h>

#include "Display.h"

using namespace Geo;

//==================================================================================================
// SdlDisplay
//==================================================================================================

void SdlDisplay::Initialize()
{
    if (!m_window) {
        LOG_INFO("Creating SDL window");

        SDL_GL_ResetAttributes();
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, RENDER_GL_MAJOR_VERSION);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, RENDER_GL_MINOR_VERSION);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        m_window = SDL_CreateWindow(
            GAME_TITLE,
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

        if (!m_window)
            FATAL("Can't create SDL window: {}", SDL_GetError());
    }

    if (!m_glContext) {
        LOG_INFO("Creating OpenGL context");

        m_glContext = SDL_GL_CreateContext(m_window);

        if (!m_glContext)
            FATAL("Can't create OpenGL context: {}", SDL_GetError());

        // Enable vsync. Eventually we'll make this configurable.
        if (SDL_GL_SetSwapInterval(-1) == 0)
            LOG_INFO("Enabled adaptive vsync");
        else if (SDL_GL_SetSwapInterval(1) == 0)
            LOG_INFO("Enabled vsync");
        else
            LOG_WARNING("Can't enable vsync: {}", SDL_GetError());
    }
}

void SdlDisplay::ShutDown()
{
    if (m_glContext) {
        SDL_GL_MakeCurrent(nullptr, nullptr);
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

Vec2i SdlDisplay::GetSize() const
{
    Vec2i size = {0, 0};

    if (m_window)
        SDL_GetWindowSize(m_window, &size.x, &size.y);

    return size;
}

void SdlDisplay::SwapBuffers()
{
    if (m_window)
        SDL_GL_SwapWindow(m_window);
}

//==================================================================================================
// Display
//==================================================================================================

void* Display::gl_GetProcAddress(const char* name)
{
    return SDL_GL_GetProcAddress(name);
}
