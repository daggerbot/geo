/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <cstdio>

#include <glad/gl.h>

#include <client/display.h>
#include <core/game_defs.h>
#include <system/debug.h>

#include "geometry.h"
#include "render.h"
#include "shaders.h"
#include "utils.h"

using namespace geo;
using namespace geo::render;

namespace geo::render::state {

    Shader current_shader = Shader::none;
    GlProgram* current_program = nullptr;
    std::shared_ptr<GeometryBase> current_geometry;

    Mat4f projection = Mat4f::identity();
    Mat4f object_transform = Mat4f::identity();

} // namespace geo::render::state

namespace {

    Vec2i window_size = {0, 0};

} // namespace

//==================================================================================================
// Initialization functions
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
        LOG_INFO("Loading OpenGL {}.{} API", gl::major_version, gl::minor_version);

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
    state::current_geometry = {};
}

//==================================================================================================
// Frame setup functions
//==================================================================================================

void render::begin_draw()
{
    window_size = display::get_window_size();
}

void render::end_draw()
{
    gl::flush_errors();
}

//==================================================================================================
// Framebuffer functions
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

//==================================================================================================
// Viewport functions
//==================================================================================================

void render::set_viewport(Recti viewport)
{
    glViewport(viewport.x0, window_size.y - viewport.y1, viewport.width(), viewport.height());
}

void render::set_full_viewport()
{
    glViewport(0, 0, window_size.x, window_size.y);
}

//==================================================================================================
// Render state functions
//==================================================================================================

void render::select_shader(Shader shader)
{
    if (shader == state::current_shader)
        return;

    state::current_shader = shader;

    switch (shader) {
        case Shader::color:
            state::current_program = &prog_color;
            break;

        default:
            FATAL("Invalid shader");
    }

    glUseProgram(state::current_program->program_id());

    // Restore the uniform state.
    glUniformMatrix4fv(uniform_index_projection, 1, GL_FALSE, &state::projection.x.x);
    glUniformMatrix4fv(uniform_index_object_transform, 1, GL_FALSE, &state::object_transform.x.x);
}

void render::select_geometry(const std::shared_ptr<GeometryBase>& geometry)
{
    ASSERT(geometry != nullptr);
    glBindVertexArray(geometry->gl().gl_vertex_array_id());
}

void render::set_projection(const Mat4f& matrix)
{
    state::projection = matrix;

    if (state::current_program)
        glUniformMatrix4fv(uniform_index_projection, 1, GL_FALSE, &matrix.x.x);
}

void render::set_object_transform(const Mat4f& matrix)
{
    state::object_transform = matrix;

    if (state::current_program)
        glUniformMatrix4fv(uniform_index_object_transform, 1, GL_FALSE, &matrix.x.x);
}

//==================================================================================================
// Drawing functions
//==================================================================================================

void render::draw_triangles(size_t first_vertex_index, size_t vertex_count)
{
    glDrawArrays(GL_TRIANGLES, GLint(first_vertex_index), GLsizei(vertex_count));
}
