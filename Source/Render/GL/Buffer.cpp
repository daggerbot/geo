/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <limits>

#include <glad/gl.h>

#include <System/Debug.h>

#include "Buffer.h"
#include "Gl.h"

using namespace Geo;

namespace {

    constexpr GLenum GetGlTarget(RenderBufferType type)
    {
        switch (type) {
            case RenderBufferType::Vertex: return GL_ARRAY_BUFFER;
            default: FATAL("Invalid RenderBufferType: {}", int(type));
        }
    }

    constexpr GLenum GetGlUsage(RenderBufferUsage usage)
    {
        switch (usage) {
            case RenderBufferUsage::StaticDraw: return GL_STATIC_DRAW;
            case RenderBufferUsage::StaticRead: return GL_STATIC_READ;
            case RenderBufferUsage::StaticCopy: return GL_STATIC_COPY;
            case RenderBufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
            case RenderBufferUsage::DynamicRead: return GL_DYNAMIC_READ;
            case RenderBufferUsage::DynamicCopy: return GL_DYNAMIC_COPY;
            case RenderBufferUsage::StreamDraw: return GL_STREAM_DRAW;
            case RenderBufferUsage::StreamRead: return GL_STREAM_READ;
            case RenderBufferUsage::StreamCopy: return GL_STREAM_COPY;
            default: FATAL("Invalid RenderBufferUsage: {}", int(usage));
        }
    }

} // namespace

GlRenderBuffer::GlRenderBuffer(size_t size, RenderBufferType type, RenderBufferUsage usage)
{
    GLenum target = GetGlTarget(type);
    GLenum gl_usage = GetGlUsage(usage);

    ASSERT(size > 0);

    Gl::FlushErrors();

    LOG_TRACE("Creating GL buffer with size {:H}", size);

    // Check for buffer size overflow.
    if constexpr(sizeof(size) >= sizeof(GLsizeiptr)) {
        if (size > decltype(size)(std::numeric_limits<GLsizeiptr>::max())) {
            return;
        }
    }

    // Create the buffer object.
    glGenBuffers(1, &m_bufferId);

    if (!m_bufferId)
        FATAL("glGenBuffers: {}", Gl::GetErrorString(glGetError()));

    // Allocate the buffer data.
    glBindBuffer(target, m_bufferId);
    glBufferData(target, GLsizeiptr(size), nullptr, gl_usage);

    if (GLenum errnum = glGetError()) {
        if (errnum == GL_OUT_OF_MEMORY) {
            LOG_WARNING("Insufficient VRAM to allocate GL buffer");
            Destroy();
            return;
        } else {
            FATAL("Failed to create GL buffer: {}", Gl::GetErrorString(errnum));
        }
    }

    // Success!
    m_type = type;
    m_size = size;
}

GlRenderBuffer::~GlRenderBuffer()
{
    Destroy();
}

void GlRenderBuffer::CopyFrom(const RenderBuffer& src, size_t srcOffset, size_t dstOffset, size_t size)
{
    if (!size)
        return;

    ASSERT(srcOffset < src.GetSize());
    ASSERT(size <= src.GetSize() - srcOffset);
    ASSERT(dstOffset < m_size);
    ASSERT(size <= m_size - dstOffset);

    if (&src == this) {
        if (srcOffset < dstOffset)
            ASSERT(dstOffset >= srcOffset + size);
        else
            ASSERT(srcOffset >= dstOffset + size);
    }

    glBindBuffer(GL_COPY_READ_BUFFER, src.gl().GetBufferId());
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_bufferId);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                        GLintptr(srcOffset), GLintptr(dstOffset), GLsizeiptr(size));

    if (GLenum errnum = glGetError())
        LOG_ERROR("Failed to copy GL buffer data: {}", Gl::GetErrorString(errnum));
}

void GlRenderBuffer::Write(const void* src, size_t dstOffset, size_t size)
{
    GLenum target;

    if (!size)
        return;

    ASSERT(src != nullptr);
    ASSERT(dstOffset < m_size);
    ASSERT(size <= m_size - dstOffset);

    target = GetGlTarget(m_type);
    glBindBuffer(target, m_bufferId);
    glBufferSubData(target, GLintptr(dstOffset), GLsizeiptr(size), src);

    if (GLenum errnum = glGetError())
        LOG_ERROR("Failed to write to GL buffer: {}", Gl::GetErrorString(errnum));
}

void GlRenderBuffer::Destroy()
{
    if (!IsValid())
        return;

    glDeleteBuffers(1, &m_bufferId);

    m_bufferId = 0;
    m_type = RenderBufferType::None;
    m_size = 0;
}
