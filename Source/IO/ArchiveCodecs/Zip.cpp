/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <errno.h>
#include <stdio.h>

#include <memory>

#include <zip.h>

#include <Core/Finally.h>
#include <System/Debug.h>

#include "Zip.h"

using namespace Geo;

//==================================================================================================
// LibzipErrorCategory
//==================================================================================================

namespace {

    class LibzipErrorCategoryType : public std::error_category {
    public:
        std::string message(int value) const override
        {
            zip_error_t zerror;
            zip_error_init_with_code(&zerror, value);
            std::string msg = zip_error_strerror(&zerror);
            zip_error_fini(&zerror);
            return msg;
        }

        const char* name() const noexcept override { return "libzip"; }
    } constinit const s_libzipErrorCategory;

    Error MakeZipError(int zerr, int cerr, const char* description)
    {
        Error error = {};

        if (description)
            error.description = description;

        if (zerr) {
            error.code = {zerr, LibzipErrorCategory};
            if (cerr)
                error.innerError.reset(new Error{.code = {cerr, std::generic_category()}});
        } else if (cerr) {
            error.code = {cerr, std::generic_category()};
        }

        return error;
    }

    Error MakeZipError(const zip_error_t* zerror, const char* description)
    {
        return MakeZipError(zip_error_code_zip(zerror), zip_error_code_system(zerror), description);
    }

} // namespace

constinit const std::error_category& Geo::LibzipErrorCategory = s_libzipErrorCategory;

//==================================================================================================
// ZipArchive
//==================================================================================================

ZipArchive::ZipArchive(ZipArchive&& other)
    : m_zip{other.m_zip}
{
    other.m_zip = nullptr;
}

ZipArchive::ZipArchive(const oschar_t* path, Error& outError)
{
    Open(path, outError);
}

ZipArchive::~ZipArchive()
{
    Close();
}

ZipArchive& ZipArchive::operator=(ZipArchive&& other)
{
    if (&other != this) {
        Close();
        m_zip = other.m_zip;
        other.m_zip = nullptr;
    }
    return *this;
}

void ZipArchive::Close()
{
    Error error;

    Close(error);

    if (error)
        LOG_ERROR("Failed to close ZIP archive: {}", error);
}

void ZipArchive::Close(Error& outError)
{
    if (!m_zip)
        return;

    if (zip_close(m_zip) != 0)
        outError = MakeZipError(zip_get_error(m_zip), "zip_close failed");

    m_zip = nullptr;
}

bool ZipArchive::Open(const oschar_t* path, Error& outError)
{
    Close();

    // Open the underlying stream.
    std::unique_ptr<FILE, decltype(&fclose)> fp{nullptr, &fclose};

#ifdef _WIN32
    fp.reset(_wfopen(path, L"rb"));
#else
    fp.reset(fopen(path, "rb"));
#endif

    if (!fp) {
        outError = {.description = "fopen failed", .code = {errno, std::generic_category()}};
        return false;
    }

    // Capture libzip errors.
    zip_error_t zerror;
    zip_error_init(&zerror);
    Finally freeZipError = [&] { zip_error_fini(&zerror); };

    // Create a zip source from the underlying stream.
    std::unique_ptr<zip_source_t, decltype(&zip_source_free)> source{nullptr, &zip_source_free};
    source.reset(zip_source_filep_create(fp.get(), 0, ZIP_LENGTH_TO_END, &zerror));

    if (!source) {
        outError = MakeZipError(&zerror, "zip_source_filep_create failed");
        return false;
    }

    fp.release(); // `fp` is now owned by `source`

    // Open the zip archive from the zip source.
    m_zip = zip_open_from_source(source.get(), ZIP_RDONLY, &zerror);

    if (!m_zip) {
        outError = MakeZipError(&zerror, "zip_open_from_source failed");
        return false;
    }

    source.release(); // `source` is now owned by `m_zip`
    return true;
}

std::unique_ptr<Stream> ZipArchive::OpenStream(const char* name, Error& outError)
{
    Error localError;
    std::unique_ptr<ZipStream> stream;

    stream = std::make_unique<ZipStream>(*this, name, localError);

    if (localError) {
        outError = std::move(localError);
        return nullptr;
    }

    return stream;
}

//==================================================================================================
// ZipStream
//==================================================================================================

ZipStream::ZipStream(ZipStream&& other)
    : m_zfp{other.m_zfp}
    , m_size{other.m_size}
{
    other.m_zfp = nullptr;
    other.m_size = -1;
}

ZipStream::ZipStream(ZipArchive& archive, const char* name, Error& outError)
{
    Open(archive, name, outError);
}

ZipStream::~ZipStream()
{
    Close();
}

void ZipStream::Close(Error& outError)
{
    if (!m_zfp)
        return;

    if (int zerr = zip_fclose(m_zfp))
        outError = MakeZipError(zerr, 0, "zip_fclose failed");

    m_zfp = nullptr;
    m_size = -1;
}

i64 ZipStream::GetSize(Error& outError) const
{
    if (!m_zfp) {
        outError = {.code = IoErrorCode::StreamClosed};
        return -1;
    }

    return m_size;
}

bool ZipStream::Open(ZipArchive& archive, const char* name, Error& outError)
{
    Close();

    if (!archive.IsOpen()) {
        outError = {.code = IoErrorCode::ArchiveClosed};
        return false;
    }

    // Locate the file entry in the archive.
    i64 index = zip_name_locate(archive.m_zip, name, 0);

    if (index < 0) {
        outError = {.code = IoErrorCode::NotFound};
        return false;
    }

    // Get the file size.
    zip_stat_t stat;

    if (zip_stat_index(archive.m_zip, index, ZIP_STAT_SIZE, &stat) != 0) {
        outError = MakeZipError(zip_get_error(archive.m_zip), "zip_stat_index failed");
        return false;
    }

    // Open the underlying stream.
    m_zfp = zip_fopen_index(archive.m_zip, index, 0);

    if (!m_zfp) {
        outError = MakeZipError(zip_get_error(archive.m_zip), "zip_fopen_index failed");
        return false;
    }

    // Success!
    m_size = stat.size;
    return true;
}

size_t ZipStream::ReadPartial(void* dst, size_t size, Error& outError)
{
    if (!size)
        return 0;

    if (!m_zfp) {
        outError = {.code = IoErrorCode::StreamClosed};
        return 0;
    }

    ASSERT(dst != nullptr);

    i64 result = zip_fread(m_zfp, dst, size);

    if (result < 0) {
        outError = MakeZipError(zip_file_get_error(m_zfp), "zip_fread failed");
        return 0;
    }

    return size_t(result);
}
