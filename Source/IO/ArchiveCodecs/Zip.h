/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_IO_ARCHIVECODECS_ZIP_H_INCLUDED
#define GEO_IO_ARCHIVECODECS_ZIP_H_INCLUDED

#include <IO/Stream.h>

struct zip;
struct zip_file;

namespace Geo {

    /// @addtogroup IO
    /// @{

    /// `std::error_category` corresponding to libzip error codes.
    extern constinit const std::error_category& LibzipErrorCategory;

    /// Reads entries from a ZIP archive.
    class ZipArchive : public StreamProvider {
        friend class ZipStream;

    public:
        ZipArchive() = default;
        ZipArchive(const ZipArchive&) = delete;
        ZipArchive(ZipArchive&& other);
        explicit ZipArchive(const oschar_t* path, Error& outError);
        ~ZipArchive();

        ZipArchive& operator=(const ZipArchive&) = delete;
        ZipArchive& operator=(ZipArchive&& other);

        void Close();
        void Close(Error& outError);
        bool IsOpen() const { return m_zip != nullptr; }
        bool Open(const oschar_t* path, Error& outError);
        std::unique_ptr<Stream> OpenStream(const char* name, Error& outError) override;

    private:
        struct ::zip* m_zip = nullptr;
    };

    /// Reads data from a ZIP archive entry.
    class ZipStream final : public Stream {
    public:
        using Stream::Close;

        ZipStream() = default;
        ZipStream(const ZipStream&) = delete;
        ZipStream(ZipStream&& other);
        explicit ZipStream(ZipArchive& archive, const char* name, Error& outError);
        ~ZipStream();

        ZipStream& operator=(const ZipStream&) = delete;
        ZipStream& operator=(ZipStream&& other);

        void Close(Error& outError) override;
        i64 GetSize(Error& outError) const override;
        bool IsOpen() const override { return m_zfp != nullptr; }
        bool IsReadable() const override { return true; }
        bool Open(ZipArchive& archive, const char* name, Error& outError);
        size_t ReadPartial(void* dst, size_t size, Error& outError) override;

    private:
        struct ::zip_file* m_zfp = nullptr;
        i64 m_size = -1;
    };

    /// @}

} // namespace Geo

#endif // GEO_IO_ARCHIVECODECS_ZIP_H_INCLUDED
