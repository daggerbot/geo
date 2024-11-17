/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_TYPES_H_INCLUDED
#define RENDER_TYPES_H_INCLUDED

#include <graphics/rgb.h>
#include <math/vector.h>

namespace geo::render {

    /// GPU program that determines how to render an object.
    enum class Shader {
        none,
        color,
    };

    /// Determines the layout of the data in a @ref GeometryBase.
    enum class VertexLayout {
        none,
        color,
    };

    /// Indicates what kind of data a @ref BufferBase holds.
    enum class BufferType {
        none,
        vertex,
    };

    /// Determines how frequently a buffer's data is used and updated.
    enum class BufferUsage {
        none,
        static_draw,
        static_read,
        static_copy,
        dynamic_draw,
        dynamic_read,
        dynamic_copy,
        stream_draw,
        stream_read,
        stream_copy,
    };

    /// Vertex type for @ref VertexLayout::color.
    struct ColorVertex {
        Vec3f position;
        Rgbaf color;
    };

} // namespace geo::render

#endif // RENDER_TYPES_H_INCLUDED
