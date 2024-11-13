/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_SYSTEM_H_INCLUDED
#define GEO_RENDER_SYSTEM_H_INCLUDED

#include <memory>

#include <Graphics/Rgb.h>
#include <Math/Matrix.h>
#include <Math/Rect.h>

#include "Types.h"

namespace Geo {

    class Display;
    class RenderBuffer;
    class RenderGeometry;

    /// @addtogroup Render
    /// @{

    /// Rendering system interface.
    class RenderSystem {
    public:
        virtual ~RenderSystem() = 0;

        RenderSystem& operator=(const RenderSystem&) = delete;

        /// Creates a buffer. Returns null if there is insufficient VRAM to allocate the buffer.
        /// All other errors are fatal.
        virtual std::shared_ptr<RenderBuffer> CreateBuffer(size_t size, RenderBufferType type, RenderBufferUsage usage) = 0;

        /// Creates a geometry descriptor.
        virtual std::shared_ptr<RenderGeometry> CreateGeometry(VertexLayout vertexLayout) = 0;

        virtual void BeginDraw(Display& display) = 0;
        virtual void EndDraw() = 0;

        virtual void ClearColorBuffer(Rgbaf color = Rgbaf::FromArgb(Color::Transparent)) = 0;
        virtual void ClearDepthBuffer(f32 depth = 0.0f) = 0;

        virtual void SetViewport(Recti viewport) = 0;
        virtual void SetFullViewport() = 0;

        virtual void SetProjection(const Mat4f& mat) = 0;
        virtual void SetObjectTransform(const Mat4f& mat) = 0;

        virtual void SelectShader(Shader shader) = 0;
        virtual void SelectGeometry(const std::shared_ptr<RenderGeometry>& geometry) = 0;

        virtual void DrawTriangles(size_t vertexStartIndex, size_t vertexCount) = 0;
    };

    inline RenderSystem::~RenderSystem() = default;

    /// @}

} // namespace Geo

#endif // GEO_RENDER_SYSTEM_H_INCLUDED
