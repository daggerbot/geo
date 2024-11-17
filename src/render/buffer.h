/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_BUFFER_H_INCLUDED
#define RENDER_BUFFER_H_INCLUDED

#include "types.h"

namespace geo::render {

    class GlBuffer;

    /// Common abstract base for `Buffer`.
    class BufferBase {
    public:
        virtual ~BufferBase() = 0;

        BufferBase& operator=(const BufferBase&) = delete;

        /// Copies data from one GPU buffer to another.
        virtual bool copy(const BufferBase& src, size_t srcOffset, size_t dstOffset, size_t size) = 0;

        /// Casts the buffer to a @ref GlBuffer.
        GlBuffer& gl();

        /// Casts the buffer to a @ref GlBuffer.
        const GlBuffer& gl() const;

        /// Indicates whether the buffer is empty, which may be true if the buffer could not be
        /// allocated due to insufficient GPU memory.
        virtual bool is_empty() const = 0;

        /// Returns the size of the buffer in bytes.
        virtual size_t size() const = 0;

        /// Copies data from the CPU to the GPU buffer.
        virtual bool write(const void* src, size_t dstOffset, size_t size) = 0;
    };

    inline BufferBase::~BufferBase() = default;

} // namespace geo::render

#include "gl/buffer.h"

#endif // RENDER_BUFFER_H_INCLUDED
