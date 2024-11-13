/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_GL_SHADERS_H_INCLUDED
#define GEO_RENDER_GL_SHADERS_H_INCLUDED

#include "Types.h"

namespace Geo {

    class StreamProvider;

    /// @addtogroup Render
    /// @{

    /// OpenGL shader step wrapper.
    class GlShader {
    public:
        GlShader() = default;
        GlShader(const GlShader&) = delete;
        ~GlShader();

        GlShader& operator=(const GlShader&) = delete;

        void Compile(StreamProvider& assets, const char* name, GLenum type);
        void Destroy();

        GLuint GetShaderId() const { return m_shaderId; }

    private:
        GLuint m_shaderId = 0;
    };

    /// OpenGL shader program wrapper.
    class GlProgram {
    public:
        GlProgram() = default;
        GlProgram(const GlProgram&) = delete;
        ~GlProgram();

        GlProgram& operator=(const GlProgram&) = delete;

        void Link(const char* name, const GlShader& vertexShader, const GlShader& fragmentShader);
        void Destroy();

        GLuint GetProgramId() const { return m_programId; }

    private:
        GLuint m_programId = 0;
    };

    /// @}

} // namespace Geo

#endif // GEO_RENDER_GL_SHADERS_H_INCLUDED
