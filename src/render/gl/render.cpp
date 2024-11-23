/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <cstdio>

#include <client/display.h>
#include <core/game_defs.h>
#include <render/render.h>
#include <system/debug.h>

#include "gl.h"
#include "shaders.h"

using namespace geo;
using namespace geo::render;

//==================================================================================================
// Subsystem management
//==================================================================================================

namespace {

    // Wrapper for display::gl_get_proc_address with a signature that gladLoadGL accepts.
    GLADapiproc require_gl_proc_address(const char* name)
    {
        void* sym = display::gl_get_proc_address(name);

        if (!sym)
            FATAL("Missing OpenGL function: {}", name);

        return reinterpret_cast<GLADapiproc>(sym);
    }

    void check_gl_version()
    {
        const char* version;
        int major = 0, minor = 0;

        if (!glGetString)
            glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(require_gl_proc_address("glGetString"));

        version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

        if (!version)
            FATAL("Missing GL_VERSION string");
        else if (std::sscanf(version, "%d.%d", &major, &minor) != 2)
            FATAL("Can't parse GL_VERSION: {}", version);
        else if (major < gl::major_version || (major == gl::major_version && minor < gl::minor_version))
            FATAL("Unsupported GL_VERSION ({}.{} needed): {}", gl::major_version, gl::minor_version, version);

        LOG_INFO("GL_VERSION: {}", version);
    }

    void load_gl_functions()
    {
        LOG_TRACE("Loading OpenGL {}.{} API", gl::major_version, gl::minor_version);

        if (!gladLoadGL(&require_gl_proc_address))
            FATAL("Failed to load OpenGL API");
    }

} // namespace

void render::init(StreamProvider& data_source)
{
    check_gl_version();
    load_gl_functions();
    init_shaders(data_source);
}

void render::shut_down()
{
}

//==================================================================================================
// Frame management
//==================================================================================================

void render::begin_draw()
{
    Vec2i window_size = display::size();

    glViewport(0, 0, window_size.x, window_size.y);
}

void render::end_draw()
{
    gl::flush_errors();
}

void render::present()
{
    display::gl_swap_buffers();
}

//==================================================================================================
// Framebuffer management
//==================================================================================================

void render::clear_color_buffer(Rgbaf color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void render::clear_depth_buffer(f32 depth)
{
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}
