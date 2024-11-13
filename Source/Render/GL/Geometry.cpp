/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <glad/gl.h>

#include <System/Debug.h>

#include "Buffer.h"
#include "Geometry.h"
#include "Gl.h"
#include "System.h"

using namespace Geo;

namespace {

    constexpr bool IsVertexLayoutValid(VertexLayout vertexLayout)
    {
        switch (vertexLayout) {
            case VertexLayout::Color:
                return true;
            default:
                return false;
        }
    }

} // namespace

GlRenderGeometry::GlRenderGeometry(VertexLayout vertexLayout)
{
    ASSERT(IsVertexLayoutValid(vertexLayout));

    Gl::FlushErrors();

    LOG_TRACE("Creating GL vertex array object");

    // Create the vertex array object.
    glGenVertexArrays(1, &m_vertexArrayId);

    if (!m_vertexArrayId)
        FATAL("glGenVertexArrays: {}", Gl::GetErrorString(glGetError()));

    glBindVertexArray(m_vertexArrayId);

    switch (vertexLayout) {
        case VertexLayout::Color:
            glEnableVertexAttribArray(Gl::PositionIdx);
            glEnableVertexAttribArray(Gl::Color_ColorIdx);
            break;

        default:
            break;
    }

    m_vertexLayout = vertexLayout;
}

GlRenderGeometry::~GlRenderGeometry()
{
    glDeleteVertexArrays(1, &m_vertexArrayId);
}

void GlRenderGeometry::SetVertexBuffer(int index, const std::shared_ptr<RenderBuffer>& vertexBuffer)
{
    GlRenderSystem& system = GlRenderSystem::GetInstance();

    ASSERT(index >= 0 && index < VertexBufferCount);
    ASSERT(vertexBuffer && vertexBuffer->GetType() == RenderBufferType::Vertex);

    glBindVertexArray(m_vertexArrayId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->gl().GetBufferId());

    switch (m_vertexLayout) {
        case VertexLayout::Color:
            ASSERT(index == 0);
            glVertexAttribPointer(Gl::PositionIdx, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, position));
            glVertexAttribPointer(Gl::Color_ColorIdx, 4, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, color));
            break;

        default:
            break;
    }

    if (GLenum errnum = glGetError())
        FATAL("Failed to set vertex buffer layout: {}", Gl::GetErrorString(errnum));

    m_vertexBuffers[index] = vertexBuffer;
}
