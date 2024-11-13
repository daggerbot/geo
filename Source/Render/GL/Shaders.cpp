/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <glad/gl.h>

#include <IO/Stream.h>
#include <System/Debug.h>

#include "Gl.h"
#include "Shaders.h"

using namespace Geo;

namespace {

    constexpr i64 MaxShaderSourceLength = 16*1024;

} // namespace

//==================================================================================================
// GlShader
//==================================================================================================

GlShader::~GlShader()
{
    Destroy();
}

void GlShader::Compile(StreamProvider& assets, const char* name, GLenum type)
{
    Destroy();
    Gl::FlushErrors();

    LOG_TRACE("Compiling shader source: {}", name);

    // Create the shader object.
    m_shaderId = glCreateShader(type);

    if (!m_shaderId)
        FATAL("glCreateShader: {}", Gl::GetErrorString(glGetError()));

    // Load the shader source.
    Error error;
    auto stream = assets.OpenStream(name, error);
    i64 sourceLength;
    std::unique_ptr<char[]> source;

    if (!stream || !stream->IsOpen())
        FATAL("{}: {}", name, error);

    sourceLength = stream->GetSize(error);

    if (sourceLength < 1) {
        if (error)
            FATAL("{}: Can't get size: {}", name, error);
        else
            FATAL("{}: Empty shader source", name);
    } else if (sourceLength > MaxShaderSourceLength) {
        FATAL("{}: Shader source too big", name);
    }

    source.reset(new char[size_t(sourceLength)]);
    stream->ReadExact(source.get(), size_t(sourceLength), error);

    if (error)
        FATAL("{}: Read failed: {}", name, error);

    // Compile the shader source.
    const GLchar* sourcePtr = source.get();
    GLint iLen = GLint(sourceLength);
    GLint compileStatus = GL_FALSE;

    glShaderSource(m_shaderId, 1, &sourcePtr, &iLen);
    glCompileShader(m_shaderId);
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus != GL_TRUE) {
        std::unique_ptr<char[]> infoLog;
        GLint infoLogLen = 0;

        glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &infoLogLen);
        infoLog.reset(new char[size_t(infoLogLen) + 1]);
        glGetShaderInfoLog(m_shaderId, infoLogLen + 1, &infoLogLen, infoLog.get());
        FATAL("{}: Failed to compile shader: {}", name, infoLog.get());
    }

    // Final checks
    if (GLenum errnum = glGetError())
        FATAL("{}: OpenGL: {}", name, Gl::GetErrorString(errnum));
}

void GlShader::Destroy()
{
    if (!m_shaderId)
        return;

    glDeleteShader(m_shaderId);

    m_shaderId = 0;
}

//==================================================================================================
// GlProgram
//==================================================================================================

GlProgram::~GlProgram()
{
    Destroy();
}

void GlProgram::Link(const char* name, const GlShader& vertexShader, const GlShader& fragmentShader)
{
    Destroy();
    Gl::FlushErrors();

    // Create the program object.
    m_programId = glCreateProgram();

    if (!m_programId)
        FATAL("glCreateProgram: {}", Gl::GetErrorString(glGetError()));

    // Link the program.
    GLint linkStatus = GL_FALSE;

    glAttachShader(m_programId, vertexShader.GetShaderId());
    glAttachShader(m_programId, fragmentShader.GetShaderId());
    glLinkProgram(m_programId);
    glGetProgramiv(m_programId, GL_LINK_STATUS, &linkStatus);

    if (linkStatus != GL_TRUE) {
        std::unique_ptr<char[]> infoLog;
        GLint infoLogLen = 0;

        glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoLogLen);
        infoLog.reset(new char[size_t(infoLogLen) + 1]);
        glGetProgramInfoLog(m_programId, infoLogLen + 1, &infoLogLen, infoLog.get());
        FATAL("{}: Failed to compile shader: {}", name, infoLog.get());
    }
}

void GlProgram::Destroy()
{
    if (!m_programId)
        return;

    glDeleteProgram(m_programId);

    m_programId = 0;
}
