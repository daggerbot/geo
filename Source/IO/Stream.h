/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_IO_STREAM_H_INCLUDED
#define GEO_IO_STREAM_H_INCLUDED

#include <memory>

#include "Error.h"

namespace Geo {

    /// @addtogroup IO
    /// @{

    /// Origin from which seek offsets are measured.
    enum class SeekOrigin {
        Set, ///< The offset is relative to the beginning of the stream.
        Current, ///< The offset is relative to the current stream position.
        End, ///< The offset is relative to the end of the stream.
    };

    /// Base class for I/O streams.
    class Stream {
    public:
        virtual ~Stream() = 0;

        Stream& operator=(const Stream&) = delete;

        /// Closes the stream. If an error occurs, it is logged.
        void Close();

        /// Closes the stream.
        virtual void Close(Error& outError) = 0;

        /// Flushes any buffered output data.
        virtual bool Flush(Error& outError);

        /// Returns the current stream position in bytes from the beginning of the stream, or
        /// returns -1 if the position cannot be determined.
        virtual i64 GetPosition(Error& outError) const;

        /// Returns the size of the stream in bytes, or -1 if the size is unknown or undefined.
        virtual i64 GetSize(Error& outError) const;

        /// Indicates whether the stream is open.
        virtual bool IsOpen() const = 0;

        /// Indicates whether the stream supports reading operations.
        virtual bool IsReadable() const { return false; }

        /// Indicates whether the stream supports seeking operations.
        virtual bool IsSeekable() const { return false; }

        /// Indicates whether the stream supports writing operations.
        virtual bool IsWritable() const { return false; }

        /// Reads up to `size` bytes from the stream into `dst`. This repeatedly calls
        /// @ref ReadPartial until the requested data has been fully read, the end of the stream is
        /// reached, or an error occurs.
        size_t Read(void* dst, size_t size, Error& outError);

        /// Attempts to read exactly `size` bytes from the stream into `dst`. This repeatedly calls
        /// @ref ReadPartial until the requested data has been fully read or an error occurs. If the
        /// end of the stream is reached before `size` bytes are read, it is treated as an error
        /// (@ref IoErrorCode::EndOfStream).
        size_t ReadExact(void* dst, size_t size, Error& outError);

        /// Reads up to `size` bytes from the stream into `dst`. This may read only a portion of the
        /// requested data to keep the implementation simple. If reading the full amount in one call
        /// is desired, use @ref Read instead.
        virtual size_t ReadPartial(void* dst, size_t size, Error& outError);

        /// Changes the stream position.
        virtual bool Seek(i64 offset, SeekOrigin origin, Error& outError);

        /// Attempts to write exactly `size` bytes to the stream from `src`. This repeatedly calls
        /// @ref WritePartial until the provided data is fully written or an error occurs.
        size_t Write(const void* src, size_t size, Error& outError);

        /// Writes up to `size` bytes to the stream from `src`. This may write only a portion of the
        /// provided data to keep the implementation simple. If writing the full amount in one call
        /// is desired, use @ref Write instead.
        virtual size_t WritePartial(const void* src, size_t size, Error& outError);
    };

    /// Interface for opening named input streams.
    class StreamProvider {
    public:
        virtual ~StreamProvider() = 0;

        StreamProvider& operator=(const StreamProvider&) = delete;

        /// Opens the named stream for reading.
        virtual std::unique_ptr<Stream> OpenStream(const char* name, Error& outError) = 0;
    };

    inline StreamProvider::~StreamProvider() = default;

    /// @}

} // namespace Geo

#endif // GEO_IO_STREAM_H_INCLUDED
