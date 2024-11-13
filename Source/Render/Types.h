/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_TYPES_H_INCLUDED
#define GEO_RENDER_TYPES_H_INCLUDED

#include <Graphics/Rgb.h>
#include <Math/Vector.h>

namespace Geo {

    /// @addtogroup Geo
    /// @{

    /// Enumeration of built-in shaders.
    enum class Shader {
        None,
        Color,
    };

    /// Enumeration of supported vertex layouts.
    enum class VertexLayout {
        None,
        Color,
    };

    /// Specifies the type of GPU buffer, defining its role in the rendering pipeline.
    enum class RenderBufferType {
        None,
        Vertex, ///< Buffer containing vertex data, such as positions, normals, or UV coordinates.
    };

    /// Specifies usage patterns for GPU buffer data, guiding memory allocation and access behavior.
    enum class RenderBufferUsage {
        None,
        StaticDraw, ///< Data is set once by the CPU and used many times by the GPU; ideal for static geometry.
        StaticRead, ///< Data is set once by the GPU and read back frequently by the CPU; useful for feedback buffers.
        StaticCopy, ///< Data is set once by the GPU and copied many times by the GPU; for internal GPU-to-GPU operations.
        DynamicDraw, ///< Data is frequently updated by the CPU and used many times by the GPU; suitable for dynamic geometry.
        DynamicRead, ///< Data is frequently updated by the GPU and read back by the CPU; used for frequently accessed feedback data.
        DynamicCopy, ///< Data is frequently updated by the GPU and copied many times by the GPU; useful for GPU-driven processes.
        StreamDraw, ///< Data is changed every frame by the CPU and used a limited number of times by the GPU; good for per-frame data.
        StreamRead, ///< Data is frequently updated by the GPU and read once or a few times by the CPU; rare in most applications.
        StreamCopy, ///< Data is frequently updated and copied by the GPU; useful for temporary data shared among GPU processes.
    };

    /// Vertex type for @ref RenderMode::Color.
    struct ColorVertex {
        Vec3f position;
        Rgbaf color;
    };

    /// @}

} // namespace Geo

#endif // GEO_RENDER_TYPES_H_INCLUDED
