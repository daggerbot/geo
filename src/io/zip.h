/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef IO_ZIP_H_INCLUDED
#define IO_ZIP_H_INCLUDED

#include "stream.h"

struct zip;
struct zip_file;

namespace geo {

    /// Reads entries from a ZIP archive.
    class ZipArchive : public StreamProvider {
        friend class ZipStream;

    public:
        ZipArchive() = default;
        ZipArchive(const ZipArchive&) = delete;
        ZipArchive(ZipArchive&& other);
        explicit ZipArchive(const OsChar* path, Error& out_error);
        ~ZipArchive();

        ZipArchive& operator=(ZipArchive&& other);

        void close();
        void close(Error& out_error);
        bool is_open() const { return zip_ != nullptr; }
        bool open(const OsChar* path, Error& out_error);

        std::unique_ptr<Stream> open_stream(const char* name, Error& out_error) override;

    private:
        struct ::zip* zip_ = nullptr;
    };

    /// Reads data from a ZIP archive entry.
    class ZipStream : public Stream {
    public:
        using Stream::close;

        ZipStream() = default;
        ZipStream(const ZipStream&) = delete;
        ZipStream(ZipStream&& other);
        explicit ZipStream(ZipArchive& archive, const char* name, Error& out_error);
        ~ZipStream();

        ZipStream& operator=(ZipStream&& other);

        bool open(ZipArchive& archive, const char* name, Error& out_error);

        void close(Error& out_error) override;
        i64 get_size(Error& out_error) const override;
        bool is_open() const override { return zfp_ != nullptr; }
        bool is_readable() const override { return true; }
        size_t read_partial(void* dst, size_t size, Error& out_error) override;

    private:
        struct ::zip_file* zfp_ = nullptr;
        i64 size_ = -1;
    };

    /// Error category corresponding to libzip error codes.
    extern constinit const std::error_category& libzip_error_category;

} // namespace geo

#endif // IO_ZIP_H_INCLUDED
