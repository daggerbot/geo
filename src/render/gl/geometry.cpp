/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <glad/gl.h>

#include <system/debug.h>

#include "buffer.h"
#include "geometry.h"
#include "shaders.h"
#include "utils.h"

using namespace geo;
using namespace geo::render;

GlGeometry::GlGeometry(VertexLayout vertex_layout)
{
    gl::flush_errors();
    glCreateVertexArrays(1, &vertex_array_id_);

    if (!vertex_array_id_)
        FATAL("glCreateVertexArray: {}", gl::strerror(glGetError()));

    this->vertex_layout_ = vertex_layout;

    switch (vertex_layout) {
        case VertexLayout::color:
            glEnableVertexArrayAttrib(vertex_array_id_, attrib_index_position);
            glEnableVertexArrayAttrib(vertex_array_id_, color_attrib_index_color);
            glVertexArrayAttribFormat(vertex_array_id_, attrib_index_position, 3, GL_FLOAT, GL_FALSE, offsetof(ColorVertex, position));
            glVertexArrayAttribFormat(vertex_array_id_, color_attrib_index_color, 4, GL_FLOAT, GL_FALSE, offsetof(ColorVertex, color));
            break;

        default:
            FATAL("Invalid vertex layout");
    }

    if (GLenum errnum = glGetError())
        FATAL("Failed to initialize VAO: {}", gl::strerror(errnum));
}

GlGeometry::~GlGeometry()
{
    glDeleteVertexArrays(1, &vertex_array_id_);
}

void GlGeometry::set_vertex_buffer(int index, const std::shared_ptr<BufferBase>& buffer)
{
    ASSERT(index >= 0 && index < max_vertex_buffers);
    ASSERT(buffer != nullptr);

    vertex_buffers_[index] = buffer;

    switch (vertex_layout_) {
        case VertexLayout::color:
            ASSERT(index == 0);
            glVertexArrayVertexBuffer(vertex_array_id_, 0, buffer->gl().gl_buffer_id(), 0, sizeof(ColorVertex));
            glVertexArrayAttribBinding(vertex_array_id_, color_attrib_index_color, 0);
            break;

        default:
            FATAL("Unimplemented vertex layout");
    }
}
