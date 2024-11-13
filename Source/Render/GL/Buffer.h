/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_GL_BUFFER_H_INCLUDED
#define GEO_RENDER_GL_BUFFER_H_INCLUDED

#include <Render/Buffer.h>

#include "Types.h"

namespace Geo {

    /// @addtogroup Render
    /// @{

    class GlRenderBuffer final : public RenderBuffer {
    public:
        GlRenderBuffer() = delete;
        GlRenderBuffer(const GlRenderBuffer&) = delete;
        explicit GlRenderBuffer(size_t size, RenderBufferType type, RenderBufferUsage usage);
        ~GlRenderBuffer();

        /// Returns the OpenGL buffer object name.
        GLuint GetBufferId() const { return m_bufferId; }

        /// Indicates whether the buffer was successfully created.
        bool IsValid() const { return m_bufferId != 0; }

        void CopyFrom(const RenderBuffer& src, size_t srcOffset, size_t dstOffset, size_t size) override;
        size_t GetSize() const override { return m_size; }
        RenderBufferType GetType() const override { return m_type; }
        void Write(const void* src, size_t dstOffset, size_t size) override;

    private:
        GLuint m_bufferId = 0;
        size_t m_size = 0;
        RenderBufferType m_type = RenderBufferType::None;

        void Destroy();
    };

    inline GlRenderBuffer& RenderBuffer::gl() { return static_cast<GlRenderBuffer&>(*this); }
    inline const GlRenderBuffer& RenderBuffer::gl() const { return static_cast<const GlRenderBuffer&>(*this); }

    /// @}

} // namespace Geo

#endif // GEO_RENDER_GL_BUFFER_H_INCLUDED
