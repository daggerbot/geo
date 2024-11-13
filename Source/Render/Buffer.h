/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_BUFFER_H_INCLUDED
#define GEO_RENDER_BUFFER_H_INCLUDED

#include "Types.h"

namespace Geo {

    class GlRenderBuffer;

    /// @addtogroup Geo
    /// @{

    /// Manages a GPU buffer for storing and transferring vertex, index, and other data types, with
    /// configurable usage and access patterns.
    class RenderBuffer {
    public:
        virtual ~RenderBuffer() = 0;

        RenderBuffer& operator=(const RenderBuffer&) = delete;

        /// Copies data from another buffer. `src` can be the same buffer as `this` as long as the
        /// source and destination regions do not overlap.
        virtual void CopyFrom(const RenderBuffer& src, size_t srcOffset, size_t dstOffset, size_t size) = 0;

        /// Returns the size of the buffer in bytes.
        virtual size_t GetSize() const = 0;

        /// Returns the buffer type.
        virtual RenderBufferType GetType() const = 0;

        /// Writes contiguous data to the buffer.
        virtual void Write(const void* src, size_t dstOffset, size_t size) = 0;

        GlRenderBuffer& gl();
        const GlRenderBuffer& gl() const;
    };

    inline RenderBuffer::~RenderBuffer() = default;

    /// @}

} // namespace Geo

#endif // GEO_RENDER_BUFFER_H_INCLUDED
