/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_GEOMETRY_H_INCLUDED
#define RENDER_GEOMETRY_H_INCLUDED

#include <memory>

#include "types.h"

namespace geo::render {

    class BufferBase;

    class GlGeometry;

    /// Manages the geometric data of an object, such as vertex buffer(s).
    class GeometryBase {
    public:
        static inline constexpr int max_vertex_buffers = 1;

        virtual ~GeometryBase() = 0;

        GeometryBase& operator=(const GeometryBase&) = delete;

        /// Casts to a @ref GlGeometry.
        GlGeometry& gl();

        /// Casts to a @ref GlGeometry.
        const GlGeometry& gl() const;

        /// Sets the vertex buffer at the specified index.
        virtual void set_vertex_buffer(int index, const std::shared_ptr<BufferBase>& buffer) = 0;
    };

    inline GeometryBase::~GeometryBase() = default;

} // namespace geo::render

#include "gl/geometry.h"

#endif // RENDER_GEOMETRY_H_INCLUDED
