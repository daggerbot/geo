/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <glad/gl.h>

#include <System/Debug.h>

#include "Gl.h"

using namespace Geo;

void Gl::FlushErrors()
{
    int count = 0;
    GLenum errnum;

    while ((errnum = glGetError()) != GL_NO_ERROR) {
        LOG_WARNING("OpenGL: {}", GetErrorString(errnum));

        if (++count == 32)
            FATAL("Too many OpenGL errors");
    }
}

std::string Gl::GetErrorString(GLenum errnum)
{
    switch (errnum) {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        default: return Format::String("OpenGL error code {}", errnum);
    }
}
