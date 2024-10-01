/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <cstdio>

#include <glad/gl.h>

#include <geo/render/gl/loader.h>
#include <geo/render/gl/version.h>
#include <geo/render/system.h>
#include <geo/system/debug.h>

using namespace geo;

namespace {

    /// Loads an OpenGL symbol. Exits if it is not found.
    GLADapiproc require_proc_address(void* user_data, const char* name)
    {
        auto loader = reinterpret_cast<GlLoader*>(user_data);
        void* sym = loader->gl_get_proc_address(name);

        if (!sym)
            FATAL("Missing required OpenGL symbol: {}", name);

        return reinterpret_cast<GLADapiproc>(sym);
    }

    /// Ensures that we're using a compatible OpenGL version.
    void check_gl_version(GlLoader& loader)
    {
        const char* version;
        int major = 0, minor = 0;

        if (!glad_glGetString)
            glad_glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(::require_proc_address(&loader, "glGetString"));

        version = reinterpret_cast<const char*>(::glad_glGetString(GL_VERSION));
        if (!version)
            FATAL("Missing GL_VERSION");

        if (std::sscanf(version, "%d.%d", &major, &minor) != 2)
            FATAL("Can't parse GL_VERSION: {}", version);

        if (major < RENDER_GL_VERSION_MAJOR
            || (major == RENDER_GL_VERSION_MAJOR && minor < RENDER_GL_VERSION_MINOR))
        {
            FATAL("Unsupported GL_VERSION (expected {}.{}): {}",
                  RENDER_GL_VERSION_MAJOR, RENDER_GL_VERSION_MINOR, version);
        }

        LOG_INFO("GL_VERSION: {}", version);
    }

    /// Loads the OpenGL function pointers (beginning with `glad_`).
    void load_gl_api(GlLoader& loader)
    {
        LOG_TRACE("Loading OpenGL API");

        if (!::gladLoadGLUserPtr(&require_proc_address, &loader))
            FATAL("Failed to load OpenGL API");
    }

} // namespace

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::initialize()
{
    DASSERT(m_glLoader != nullptr);
    ::check_gl_version(*m_glLoader);
    ::load_gl_api(*m_glLoader);
}

void RenderSystem::shut_down()
{
}

void RenderSystem::clear(Col4f color)
{
    ::glad_glClearColor(color.r, color.g, color.g, color.a);
    ::glad_glClear(GL_COLOR_BUFFER_BIT);
}

RenderSystem& RenderSystem::get()
{
    static RenderSystem instance;
    return instance;
}
