/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <io/stream.h>
#include <system/debug.h>

#include "gl.h"
#include "shaders.h"

using namespace geo;
using namespace geo::render;

namespace {

    constexpr size_t max_shader_source_size = 16*1024;

} // namespace

//==================================================================================================
// GlShader
//==================================================================================================

GlShader::~GlShader()
{
    if (shader_id_)
        glDeleteShader(shader_id_);
}

void GlShader::compile(StreamProvider& data_source, const char* name, GLenum type)
{
    gl::flush_errors();

    LOG_TRACE("Compiling shader: {}", name);

    // Create the shader object.
    shader_id_ = glCreateShader(type);

    if (!shader_id_)
        FATAL("glCreateShader: {}", gl::strerror(glGetError()));

    // Load the shader source.
    Error error;
    auto source = data_source.read_stream_bytes(name, max_shader_source_size, error);

    if (error)
        FATAL("{}: {}", name, error);

    // Compile the shader source.
    const GLchar* source_ptr = reinterpret_cast<const GLchar*>(source.data());
    GLsizei source_len = GLsizei(source.size());
    GLint compile_status = GL_FALSE;

    glShaderSource(shader_id_, 1, &source_ptr, &source_len);
    glCompileShader(shader_id_);
    glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &compile_status);

    if (compile_status != GL_TRUE) {
        GLint info_log_len = 0;
        std::unique_ptr<char[]> info_log;

        glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &info_log_len);
        info_log.reset(new char[info_log_len + 1]);
        glGetShaderInfoLog(shader_id_, info_log_len + 1, &info_log_len, info_log.get());
        FATAL("{}: {}", name, info_log.get());
    }

    // Check for any other errors.
    if (GLenum errnum = glGetError())
        FATAL("{}: {}", name, gl::strerror(errnum));
}

//==================================================================================================
// GlProgram
//==================================================================================================

GlProgram::~GlProgram()
{
    if (program_id_ && glDeleteProgram)
        glDeleteProgram(program_id_);
}

void GlProgram::link(const char* name, GlShader& vertex_shader, GlShader& fragment_shader)
{
    gl::flush_errors();

    LOG_TRACE("Linking shader program: {}", name);

    // Create the program object.
    program_id_ = glCreateProgram();

    if (!program_id_)
        FATAL("glCreateProgram: {}", gl::strerror(glGetError()));

    // Link the program.
    GLint link_status = GL_FALSE;

    glAttachShader(program_id_, vertex_shader.shader_id());
    glAttachShader(program_id_, fragment_shader.shader_id());
    glLinkProgram(program_id_);
    glGetProgramiv(program_id_, GL_LINK_STATUS, &link_status);

    if (link_status != GL_TRUE) {
        GLint info_log_len = 0;
        std::unique_ptr<char[]> info_log;

        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &info_log_len);
        info_log.reset(new char[info_log_len + 1]);
        glGetProgramInfoLog(program_id_, info_log_len + 1, &info_log_len, info_log.get());
        FATAL("{}: {}", name, info_log.get());
    }

    // Check for any other errors.
    if (GLenum errnum = glGetError())
        FATAL("{}: {}", name, gl::strerror(errnum));
}

//==================================================================================================
// render
//==================================================================================================

GlProgram render::prog_color;

void render::init_shaders(StreamProvider& data_source)
{
    GlShader vert_color;
    GlShader frag_color;

    vert_color.compile(data_source, "shaders/gl/color.vert", GL_VERTEX_SHADER);

    frag_color.compile(data_source, "shaders/gl/color.frag", GL_FRAGMENT_SHADER);

    prog_color.link("prog_color", vert_color, frag_color);
}
