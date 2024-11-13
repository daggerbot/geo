/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_GL_GEOMETRY_H_INCLUDED
#define GEO_RENDER_GL_GEOMETRY_H_INCLUDED

#include <Render/Geometry.h>

#include "Types.h"

namespace Geo {

    class RenderBuffer;

    /// @addtogroup Render
    /// @{

    class GlRenderGeometry final : public RenderGeometry {
    public:
        GlRenderGeometry() = delete;
        GlRenderGeometry(const GlRenderGeometry&) = delete;
        explicit GlRenderGeometry(VertexLayout vertexLayout);
        ~GlRenderGeometry();

        /// Returns the OpenGL VAO name.
        GLuint GetVertexArrayId() const { return m_vertexArrayId; }

        VertexLayout GetVertexLayout() const override { return m_vertexLayout; }
        void SetVertexBuffer(int index, const std::shared_ptr<RenderBuffer>& vertexBuffer) override;

    private:
        GLuint m_vertexArrayId = 0;
        VertexLayout m_vertexLayout = VertexLayout::None;
        std::shared_ptr<RenderBuffer> m_vertexBuffers[VertexBufferCount] = {};
    };

    inline GlRenderGeometry& RenderGeometry::gl() { return static_cast<GlRenderGeometry&>(*this); }
    inline const GlRenderGeometry& RenderGeometry::gl() const { return static_cast<const GlRenderGeometry&>(*this); }

    /// @}

} // namespace Geo

#endif // GEO_RENDER_GL_GEOMETRY_H_INCLUDED
