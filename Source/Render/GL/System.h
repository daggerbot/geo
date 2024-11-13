/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_GL_SYSTEM_H_INCLUDED
#define GEO_RENDER_GL_SYSTEM_H_INCLUDED

#include <Render/System.h>

#include "Shaders.h"

namespace Geo {

    class StreamProvider;

    /// @addtogroup Render
    /// @{

    /// OpenGL rendering system.
    class GlRenderSystem : public RenderSystem {
    public:
        GlRenderSystem();
        ~GlRenderSystem();

        void Initialize(StreamProvider& assets);
        void ShutDown();

        static GlRenderSystem& GetInstance();

        std::shared_ptr<RenderBuffer> CreateBuffer(size_t size, RenderBufferType type, RenderBufferUsage usage) override;
        std::shared_ptr<RenderGeometry> CreateGeometry(VertexLayout vertexLayout) override;

        void BeginDraw(Display& display) override;
        void EndDraw() override;

        void ClearColorBuffer(Rgbaf color = Rgbaf::FromArgb(Color::Transparent)) override;
        void ClearDepthBuffer(f32 depth = 0.0f) override;

        void SetViewport(Recti viewport) override;
        void SetFullViewport() override;

        void SetProjection(const Mat4f& mat) override;
        void SetObjectTransform(const Mat4f& mat) override;

        void SelectShader(Shader shader) override;
        void SelectGeometry(const std::shared_ptr<RenderGeometry>& geometry) override;

        void DrawTriangles(size_t vertexStartIndex, size_t vertexCount) override;

    private:
        Vec2i m_windowSize = {0, 0};

        // Render state
        Shader m_currentShader = Shader::None;
        GlProgram* m_currentProgram = nullptr;
        Mat4f m_projection = Mat4f::Identity();
        Mat4f m_objectTransform = Mat4f::Identity();
        std::shared_ptr<RenderGeometry> m_currentGeometry;

        // Shader programs
        GlProgram m_prog_color;

        void InitShaders(StreamProvider& assets);
    };

    /// @}

} // namespace Geo

#endif // GEO_RENDER_GL_SYSTEM_H_INCLUDED
