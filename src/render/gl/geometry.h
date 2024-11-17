/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_GL_GEOMETRY_H_INCLUDED
#define RENDER_GL_GEOMETRY_H_INCLUDED

#include <render/geometry.h>

#include "types.h"

namespace geo::render {

    class GlGeometry : public GeometryBase {
    public:
        GlGeometry() = delete;
        GlGeometry(const GlGeometry&) = delete;
        explicit GlGeometry(VertexLayout vertex_layout);
        ~GlGeometry();

        void set_vertex_buffer(int index, const std::shared_ptr<BufferBase>& buffer) override;

        GLuint gl_vertex_array_id() const { return vertex_array_id_; }

    private:
        GLuint vertex_array_id_ = 0;
        VertexLayout vertex_layout_ = VertexLayout::none;
        std::shared_ptr<BufferBase> vertex_buffers_[max_vertex_buffers];
    };

    using Geometry = GlGeometry;

    inline GlGeometry& GeometryBase::gl() { return static_cast<GlGeometry&>(*this); }
    inline const GlGeometry& GeometryBase::gl() const { return static_cast<const GlGeometry&>(*this); }

} // namespace geo::render

#endif // RENDER_GL_GEOMETRY_H_INCLUDED
