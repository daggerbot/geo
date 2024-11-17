/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_GL_BUFFER_H_INCLUDED
#define RENDER_GL_BUFFER_H_INCLUDED

#include <render/buffer.h>

#include "types.h"

namespace geo::render {

    /// OpenGL GPU buffer.
    class GlBuffer final : public BufferBase {
    public:
        GlBuffer() = delete;
        GlBuffer(const GlBuffer&) = delete;
        explicit GlBuffer(size_t size, BufferType type, BufferUsage usage);
        ~GlBuffer();

        bool copy(const BufferBase& src, size_t srcOffset, size_t dstOffset, size_t size) override;
        bool is_empty() const override { return !size_; }
        size_t size() const override { return size_; }
        bool write(const void* src, size_t dstOffset, size_t size) override;

        GLuint gl_buffer_id() const { return buffer_id_; }

    private:
        GLuint buffer_id_ = 0;
        size_t size_ = 0;
    };

    using Buffer = GlBuffer;

    inline GlBuffer& BufferBase::gl() { return static_cast<GlBuffer&>(*this); }
    inline const GlBuffer& BufferBase::gl() const { return static_cast<const GlBuffer&>(*this); }

} // namespace geo::render

#endif // RENDER_GL_BUFFER_H_INCLUDED
