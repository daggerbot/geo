/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <limits>

#include <glad/gl.h>

#include <system/debug.h>

#include "buffer.h"
#include "utils.h"

using namespace geo;
using namespace geo::render;

namespace {

    GLenum to_gl_usage(render::BufferUsage usage)
    {
        switch (usage) {
            case BufferUsage::static_draw: return GL_STATIC_DRAW;
            case BufferUsage::static_read: return GL_STATIC_READ;
            case BufferUsage::static_copy: return GL_STATIC_COPY;
            case BufferUsage::dynamic_draw: return GL_DYNAMIC_DRAW;
            case BufferUsage::dynamic_read: return GL_DYNAMIC_READ;
            case BufferUsage::dynamic_copy: return GL_DYNAMIC_COPY;
            case BufferUsage::stream_draw: return GL_STREAM_DRAW;
            case BufferUsage::stream_read: return GL_STREAM_READ;
            case BufferUsage::stream_copy: return GL_STREAM_COPY;
            default: return 0;
        }
    }

} // namespace

GlBuffer::GlBuffer(size_t size, BufferType, BufferUsage usage)
{
    GLenum errnum;

    gl::flush_errors();

    // Check the requested buffer size.
    if (!size)
        return;

    if constexpr(sizeof(size_t) >= sizeof(GLsizeiptr)) {
        if (size > size_t(std::numeric_limits<GLsizeiptr>::max())) {
            LOG_ERROR("OpenGL buffer size overflow");
            return;
        }
    }

    // Create the buffer object.
    glCreateBuffers(1, &buffer_id_);

    if (!buffer_id_) {
        errnum = glGetError();

        if (errnum == GL_OUT_OF_MEMORY)
            return;
        else
            FATAL("glCreateBuffers: {}", gl::strerror(errnum));
    }

    // Allocate the buffer data.
    glNamedBufferData(buffer_id_, GLsizeiptr(size), nullptr, to_gl_usage(usage));
    errnum = glGetError();

    if (errnum == GL_OUT_OF_MEMORY) {
        glDeleteBuffers(1, &buffer_id_);
        buffer_id_ = 0;
    } else if (errnum != GL_NO_ERROR) {
        FATAL("glNamedBufferData: {}", gl::strerror(errnum));
    }

    // Success!
    this->size_ = size;
}

GlBuffer::~GlBuffer()
{
    if (buffer_id_)
        glDeleteBuffers(1, &buffer_id_);
}

bool GlBuffer::copy(const BufferBase& src, size_t srcOffset, size_t dstOffset, size_t size)
{
    GLenum errnum;

    gl::flush_errors();
    glCopyNamedBufferSubData(src.gl().buffer_id_, buffer_id_,
                             GLintptr(srcOffset), GLintptr(dstOffset), GLsizei(size));
    errnum = glGetError();

    if (errnum) {
        LOG_ERROR("glCopyNamedBufferSubData: {}", gl::strerror(errnum));
        return false;
    }

    return true;
}

bool GlBuffer::write(const void* src, size_t dstOffset, size_t size)
{
    GLenum errnum;

    gl::flush_errors();
    glNamedBufferSubData(buffer_id_, GLintptr(dstOffset), GLsizeiptr(size), src);
    errnum = glGetError();

    if (errnum) {
        LOG_ERROR("glNamedBufferSubData: {}", gl::strerror(errnum));
        return false;
    }

    return true;
}
