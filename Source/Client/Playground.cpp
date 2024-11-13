/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <Render/Buffer.h>
#include <Render/Geometry.h>
#include <Render/System.h>
#include <System/Debug.h>

#include "Playground.h"

using namespace Geo;

void Playground::BeginState(Client& client)
{
    static const ColorVertex triangle[] = {
        {{-0.9f, 0.9f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.0f, -0.9f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{0.9f, 0.9f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    };

    auto& render = client.GetRenderSystem();

    // buffer
    auto buffer = render.CreateBuffer(sizeof(triangle), RenderBufferType::Vertex, RenderBufferUsage::StaticDraw);

    if (!buffer)
        FATAL("Failed to allocate vertex buffer");

    buffer->Write(triangle, 0, sizeof(triangle));

    // m_geometry
    m_geometry = render.CreateGeometry(VertexLayout::Color);
    m_geometry->SetVertexBuffer(0, buffer);
}

void Playground::Render(Client&, RenderSystem& render, u32)
{
    render.ClearColorBuffer();
    render.SetFullViewport();
    render.SelectShader(Shader::Color);
    render.SelectGeometry(m_geometry);
    render.DrawTriangles(0, 3);
}
