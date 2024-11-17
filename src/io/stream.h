/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef IO_STREAM_H_INCLUDED
#define IO_STREAM_H_INCLUDED

#include <memory>
#include <vector>

#include "error.h"

namespace geo {

    /// Origin from which stream-seeking operations are based.
    enum SeekOrigin {
        set, ///< Offset is relative to the start of the stream.
        current, ///< Offset is relative to the current stream position.
        end, ///< Offset is relative to the end of the stream.
    };

    /// Base class for I/O streams.
    class Stream {
    public:
        virtual ~Stream() = 0;

        Stream& operator=(const Stream&) = delete;

        /// Closes the stream. If an error occurs, it is logged.
        void close();

        /// Closes the stream. If an error occurs, it is captured by `out_error`.
        virtual void close(Error& out_error) = 0;

        /// Writes any pending data in the output buffer.
        virtual bool flush(Error& out_error);

        /// Returns the current stream position in bytes from the start of the stream, or -1 if the
        /// position cannot be determined.
        virtual i64 get_position(Error& out_error) const;

        /// Returns the size of the stream in bytes, or -1 if the size is undefined or cannot be
        /// determined.
        virtual i64 get_size(Error& out_error) const;

        virtual bool is_open() const = 0;
        virtual bool is_readable() const { return false; }
        virtual bool is_seekable() const { return false; }
        virtual bool is_writable() const { return false; }

        /// Reads up to `size` bytes from the stream into `dst`. Repeatedly calls @ref read_partial
        /// until the buffer is completely filled, the end of the stream is reached, or an error
        /// occurs.
        size_t read(void* dst, size_t size, Error& out_error);

        /// Attempts to read exactly `size` bytes from the stream into `dst`. Repeatedly calls
        /// @ref read_partial until the buffer is completely filled or an error occurs. Unlike
        /// @ref read, this sets `out_error` to @ref IoErrorCode::end_of_stream if the end of the
        /// stream is reached before the buffer is completely filled.
        size_t read_exact(void* dst, size_t size, Error& out_error);

        /// Reads up to `size` bytes from the stream into `dst`. This may read less than the
        /// requested number of bytes to keep the implementation simple. If completely filling the
        /// buffer in one call is desired, use @ref read instead.
        virtual size_t read_partial(void* dst, size_t size, Error& out_error);

        /// Attempts to write exactly `size` bytes from `src` into the stream. Repeatedly calls
        /// @ref write_partial until the full buffer is written or an error occurs.
        size_t write(const void* src, size_t size, Error& out_error);

        /// Writes up to `size` bytes from `src` into the stream. This may write less than the
        /// requested number of bytes to keep the implementation simple. If writing the entire
        /// buffer in one call is desired, use @ref write instead.
        virtual size_t write_partial(const void* src, size_t size, Error& out_error);
    };

    /// Interface for opening named input streams.
    class StreamProvider {
    public:
        virtual ~StreamProvider() = 0;

        StreamProvider& operator=(const StreamProvider&) = delete;

        /// Opens a named input stream.
        virtual std::unique_ptr<Stream> open_stream(const char* name, Error& out_error) = 0;

        /// Opens and reads a named input stream.
        std::vector<u8> read_stream_bytes(const char* name, size_t max_size, Error& out_error);
    };

} // namespace geo

#endif // IO_STREAM_H_INCLUDED
