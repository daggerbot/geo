/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_GL_SHADERS_H_INCLUDED
#define RENDER_GL_SHADERS_H_INCLUDED

#include "types.h"

namespace geo {

    class StreamProvider;

    namespace render {

        // Uniform indices
        inline constexpr GLuint uniform_index_projection = 0;
        inline constexpr GLuint uniform_index_object_transform = 1;

        // Attribute indices
        inline constexpr GLuint attrib_index_position = 0;
        inline constexpr GLuint color_attrib_index_color = 1;

        /// OpenGL shader step wrapper.
        class GlShader {
        public:
            GlShader() = default;
            GlShader(const GlShader&) = delete;
            ~GlShader();

            GlShader& operator=(const GlShader&) = delete;

            void compile(StreamProvider& data_source, const char* name, GLenum type);
            GLuint shader_id() const { return shader_id_; }

        private:
            GLuint shader_id_ = 0;
        };

        /// OpenGL shader program wrapper.
        class GlProgram {
        public:
            GlProgram() = default;
            GlProgram(const GlProgram&) = delete;
            ~GlProgram();

            GlProgram& operator=(const GlProgram&) = delete;

            void link(const char* name, GlShader& vertex_shader, GlShader& fragment_shader);
            GLuint program_id() const { return program_id_; }

        private:
            GLuint program_id_ = 0;
        };

        // All shader programs
        extern GlProgram prog_color;

        void init_shaders(StreamProvider& data_source);

    } // namespace render

} // namespace geo

#endif // RENDER_GL_SHADERS_H_INCLUDED
