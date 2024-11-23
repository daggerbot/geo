/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <system/debug.h>

#include "gl.h"

using namespace geo;

void gl::flush_errors()
{
    GLenum errnum;
    int num_errors = 0;

    while ((errnum = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR("OpenGL: {}", gl::strerror(errnum));

        if (++num_errors == 50)
            FATAL("Too many OpenGL errors");
    }
}

std::string gl::strerror(GLenum errnum)
{
    switch (errnum) {
        case GL_NO_ERROR: return "No error";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
        default: return fmt::format("OpenGL error code 0x{:x}", errnum);
    }
}
