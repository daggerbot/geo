/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <cstdio>

#include <glad/gl.h>

#include <Client/Display.h>
#include <Core/GameDefs.h>
#include <System/Debug.h>

#include "Buffer.h"
#include "Geometry.h"
#include "Gl.h"
#include "Shaders.h"
#include "System.h"

using namespace Geo;

namespace {

    GlRenderSystem* s_renderSystem = nullptr;

    constexpr bool ShaderMatchesVertexLayout(Shader shader, VertexLayout layout)
    {
        switch (shader) {
            case Shader::Color:
                return layout == VertexLayout::Color;
            default:
                return false;
        }
    }

} // namespace

//==================================================================================================
// OpenGL API functions
//==================================================================================================

namespace {

    // Gets an OpenGL API function pointer, or fails if it isn't found.
    GLADapiproc RequireGlProcAddress(const char* name)
    {
        void* sym = Display::gl_GetProcAddress(name);

        if (!sym)
            FATAL("Missing OpenGL function: {}", name);

        return reinterpret_cast<GLADapiproc>(sym);
    }

    // Ensures that the OpenGL context version is compatible with our code.
    void CheckGlVersion()
    {
        const char* version;
        int major = 0, minor = 0;

        if (!glGetString)
            glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(RequireGlProcAddress("glGetString"));

        version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

        if (!version)
            FATAL("Missing GL_VERSION string");
        else if (std::sscanf(version, "%d.%d", &major, &minor) != 2)
            FATAL("Can't parse GL_VERSION string: {}", version);

        if (major < RENDER_GL_MAJOR_VERSION
            || (major == RENDER_GL_MAJOR_VERSION && minor < RENDER_GL_MINOR_VERSION))
        {
            FATAL("Unsupported GL_VERSION ({}.{} needed): {}",
                  RENDER_GL_MAJOR_VERSION, RENDER_GL_MINOR_VERSION, version);
        }

        LOG_INFO("GL_VERSION: {}", version);
    }

    // Loads the OpenGL function pointers.
    void LoadGlApi()
    {
        LOG_INFO("Loading OpenGL {}.{} API", RENDER_GL_MAJOR_VERSION, RENDER_GL_MINOR_VERSION);

        if (!gladLoadGL(&RequireGlProcAddress))
            FATAL("Failed to load OpenGL API");
    }

} // namespace

//==================================================================================================
// GlRenderSystem public functions
//==================================================================================================

GlRenderSystem::GlRenderSystem()
{
    ASSERT(s_renderSystem == nullptr);
    s_renderSystem = this;
}

GlRenderSystem::~GlRenderSystem()
{
    s_renderSystem = nullptr;
}

void GlRenderSystem::Initialize(StreamProvider& assets)
{
    CheckGlVersion();
    LoadGlApi();
    InitShaders(assets);
}

void GlRenderSystem::ShutDown()
{
}

GlRenderSystem& GlRenderSystem::GetInstance()
{
    ASSERT(s_renderSystem != nullptr);
    return *s_renderSystem;
}

std::shared_ptr<RenderBuffer> GlRenderSystem::CreateBuffer(size_t size, RenderBufferType type, RenderBufferUsage usage)
{
    auto buffer = std::make_shared<GlRenderBuffer>(size, type, usage);

    if (buffer->IsValid())
        return buffer;
    else
        return {};
}

std::shared_ptr<RenderGeometry> GlRenderSystem::CreateGeometry(VertexLayout vertexLayout)
{
    return std::make_shared<GlRenderGeometry>(vertexLayout);
}

void GlRenderSystem::BeginDraw(Display& display)
{
    m_windowSize = display.GetSize();
}

void GlRenderSystem::EndDraw()
{
    Gl::FlushErrors();
}

void GlRenderSystem::ClearColorBuffer(Rgbaf color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GlRenderSystem::ClearDepthBuffer(f32 depth)
{
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GlRenderSystem::SetViewport(Recti viewport)
{
    glViewport(viewport.x0, m_windowSize.y - viewport.y1, viewport.GetWidth(), viewport.GetHeight());
}

void GlRenderSystem::SetFullViewport()
{
    glViewport(0, 0, m_windowSize.x, m_windowSize.y);
}

void GlRenderSystem::SetProjection(const Mat4f& mat)
{
    m_projection = mat;

    if (m_currentProgram)
        glUniformMatrix4fv(Gl::ProjectionIdx, 1, GL_FALSE, &m_projection.x.x);
}

void GlRenderSystem::SetObjectTransform(const Mat4f& mat)
{
    m_objectTransform = mat;

    if (m_currentProgram)
        glUniformMatrix4fv(Gl::ObjectTransformIdx, 1, GL_FALSE, &m_objectTransform.x.x);
}

void GlRenderSystem::SelectShader(Shader shader)
{
    if (shader == m_currentShader)
        return;

    switch (shader) {
        case Shader::Color:
            m_currentProgram = &m_prog_color;
            break;
        default:
            LOG_ERROR("Invalid shader #{}", int(shader));
            return;
    }

    m_currentShader = shader;

    glUseProgram(m_currentProgram->GetProgramId());

    // Restore the uniform state.
    glUniformMatrix4fv(Gl::ProjectionIdx, 1, GL_FALSE, &m_projection.x.x);
    glUniformMatrix4fv(Gl::ObjectTransformIdx, 1, GL_FALSE, &m_objectTransform.x.x);
}

void GlRenderSystem::SelectGeometry(const std::shared_ptr<RenderGeometry>& geometry)
{
    ASSERT(geometry != nullptr);
    ASSERT(ShaderMatchesVertexLayout(m_currentShader, geometry->GetVertexLayout()));

    m_currentGeometry = geometry;

    glBindVertexArray(geometry->gl().GetVertexArrayId());
}

void GlRenderSystem::DrawTriangles(size_t vertexStartIndex, size_t vertexCount)
{
    glDrawArrays(GL_TRIANGLES, GLint(vertexStartIndex), GLsizei(vertexCount));
}

//==================================================================================================
// GlRenderSystem private functions
//==================================================================================================

void GlRenderSystem::InitShaders(StreamProvider& assets)
{
    GlShader color_vert;
    GlShader color_frag;

    color_vert.Compile(assets, "Shaders/GL/Color.vert", GL_VERTEX_SHADER);

    color_frag.Compile(assets, "Shaders/GL/Color.frag", GL_FRAGMENT_SHADER);

    m_prog_color.Link("m_prog_color", color_vert, color_frag);
}
