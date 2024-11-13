/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_GEOMETRY_H_INCLUDED
#define GEO_RENDER_GEOMETRY_H_INCLUDED

#include <memory>

#include "Types.h"

namespace Geo {

    class GlRenderGeometry;
    class RenderBuffer;

    /// @addtogroup Render
    /// @{

    /// Describes the vertex data associated with a renderable object.
    class RenderGeometry {
    public:
        static inline constexpr u8 VertexBufferCount = 1;

        virtual ~RenderGeometry() = 0;

        RenderGeometry& operator=(const RenderGeometry&) = delete;

        /// Returns the vertex layout that was specified when creating the geometry.
        virtual VertexLayout GetVertexLayout() const = 0;

        /// Sets the vertex buffer at the specified index. The meaning of the index is determined by
        /// the vertex layout. Most vertex layouts only expect one vertex buffer, so use index 0 in
        /// most cases.
        virtual void SetVertexBuffer(int index, const std::shared_ptr<RenderBuffer>& vertexBuffer) = 0;

        GlRenderGeometry& gl();
        const GlRenderGeometry& gl() const;
    };

    inline RenderGeometry::~RenderGeometry() = default;

    /// @}

} // namespace Geo

#endif // GEO_RENDER_GEOMETRY_H_INCLUDED
